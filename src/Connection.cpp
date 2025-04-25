#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "DataAdapter.hpp"
#include "HttpProcessor.hpp"
#include "Utils.hpp"

Connection::Connection(Socket& socket) : _socket(socket), _dataAdapter(NULL), _cgiAdapter(NULL) {
	hasServerAssigned = false;
	isOverPayloadLimit = false;
	hasPendingCgi = false;
	hasChunksEnded = false;
	requestMode = SINGLE;
	responseMode = NORMAL;
	contentLength = 0;
	recvBuffer.clear();
	sendBuffer.clear();
	boundarie.clear();

}

Connection::Connection(const Connection& src) : _socket(src._socket) {
	_server = src._server;
	recvBuffer = src.recvBuffer;
	sendBuffer = src.sendBuffer;
	isOverPayloadLimit = src.isOverPayloadLimit;
	hasServerAssigned =  src.hasServerAssigned;
	hasPendingCgi = src.hasPendingCgi;
	hasChunksEnded = src.hasChunksEnded;
	responseMode = src.responseMode;
	requestMode = src.requestMode;
	boundarie = src.boundarie;
	contentLength = src.contentLength;
}

Connection& Connection::operator=(const Connection& src) {
	if (this != &src) {
		_server = src._server;
		_socket = src._socket;
		recvBuffer = src.recvBuffer;
		sendBuffer = src.sendBuffer;
		isOverPayloadLimit = src.isOverPayloadLimit;
		hasServerAssigned =  src.hasServerAssigned;
		hasPendingCgi = src.hasPendingCgi;
		hasChunksEnded = src.hasChunksEnded;
		responseMode = src.responseMode;
		requestMode = src.requestMode;
		boundarie = src.boundarie;
		contentLength = src.contentLength;
	}
	return *this;
}

Connection::~Connection() {
	if(_dataAdapter != NULL)
		delete _dataAdapter;
	if (_cgiAdapter != NULL)
		delete _cgiAdapter;
	close(_socket.getPollFd().fd);
}

Socket&	Connection::getSocket() const { return _socket; }
Server	Connection::getServer() const { return _server; }

void	Connection::setServer(Server& server) { _server = server; }

static void	checkBinaryDownload(HttpRequest& request) {
	size_t downloadParamPos = request.url.find("?download=true");
	if (downloadParamPos != std::string::npos) {
		request.url = request.url.substr(0, downloadParamPos);
		request.isBinaryDownload =  true;
	}
}

void	Connection::resetConnection() {
	delete _dataAdapter;
	_dataAdapter = NULL;
	delete _cgiAdapter;
	_cgiAdapter = NULL;

	hasServerAssigned = false;
	isOverPayloadLimit = false;
	hasPendingCgi = false;
	hasChunksEnded = false;
	boundarie.clear();
	boundStart.clear();
	boundEnd.clear();
	requestMode = SINGLE;
	responseMode = NORMAL;
	contentLength = 0;
}

void	Connection::manageSingle(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter){
	
	if (!hasPendingCgi) {
		dataAdapter.deserializeRequest();

		std::cout	<< BLUE << "Info: Server '" << dataAdapter.getConnection()->getServer().getName()
					<< "' requested url '" << dataAdapter.getRequest().url << "'" << END << std::endl;
	}

	dataAdapter.getRequest().isCgiRequest 
		= CgiAdapter::isCgiRequest(dataAdapter.getRequest().url);
	checkBinaryDownload(dataAdapter.getRequest());

	HttpProcessor::processHttpRequest(dataAdapter, cgiAdapter);

	dataAdapter.serializeResponse();

	
	if (requestMode == Connection::PARTS) {
		dataAdapter.getResponse().statusCode = "";
		if (!hasPendingCgi && contentLength == 0)
			resetConnection();
	}
	else if (requestMode == Connection::CHUNKS) {
		dataAdapter.getResponse().statusCode = "";
		if (!hasPendingCgi && hasChunksEnded )
			resetConnection();
	}
	else if (!hasPendingCgi && contentLength == 0)
		resetConnection();
	recvBuffer.clear();
}

void	Connection::manageMultiPart(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter){

	dataAdapter.deserializeRequest();
		
	dataAdapter.getRequest().method = "POST";
	dataAdapter.getRequest().isCgiRequest = CgiAdapter::isCgiRequest(dataAdapter.getRequest().url);
	checkBinaryDownload(dataAdapter.getRequest());

	if (requestMode == CHUNKS && !hasChunksEnded)
		return;

	HttpProcessor::processHttpRequest(dataAdapter, cgiAdapter);
	
	if (!dataAdapter.getResponse().statusCode.empty())
		dataAdapter.serializeResponse();
	recvBuffer.clear();
	dataAdapter.getRequest().body.clear();

	if (requestMode == Connection::PARTS) {
		dataAdapter.getResponse().statusCode = "";
		if (!hasPendingCgi && contentLength == 0)
			resetConnection();
	}
	else if (requestMode == Connection::CHUNKS) {
		dataAdapter.getResponse().statusCode = "";
		if (!hasPendingCgi && hasChunksEnded )
			resetConnection();
	}
	else if (!hasPendingCgi && contentLength == 0)
		resetConnection();
}

void	Connection::fetch() {
	if (requestMode != SINGLE)
		manageMultiPart(*_dataAdapter, *_cgiAdapter);
	else 
		manageSingle(*_dataAdapter, *_cgiAdapter);
}

void	Connection::recieveData() {
	char	buffer[READ_BUFFER] = {0};
	int		socketFd = _socket.getPollFd().fd;
	int		len;

	if (_dataAdapter == NULL)
		_dataAdapter = new DataAdapter(this);
	if (_cgiAdapter == NULL)
		_cgiAdapter = new CgiAdapter();

	len = recv(socketFd, buffer, READ_BUFFER, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << socketFd << END << std::endl;
		ConnectionManager::deleteConnection(this);
	}
	else if (len == 0) {
		std::cout << BLUE << "Info: client closed connection " << socketFd << END << std::endl;
		ConnectionManager::deleteConnection(this);
	}
	else if (len > 0) {
		recvBuffer.assign(buffer, buffer + len);
		if (requestMode != SINGLE) {
			if (requestMode == PARTS)
				contentLength -= len;
			manageMultiPart(*_dataAdapter, *_cgiAdapter);
		}
		else 
			manageSingle(*_dataAdapter, *_cgiAdapter);
	}
}

void	Connection::sendData() {
	std::string	chunk;
	static bool	chunkHeadSent = false;
	std::string	chunkBuffer = std::string(sendBuffer.begin(), sendBuffer.end());
	int			socketFd = _socket.getPollFd().fd;

	chunk.clear();
	if (responseMode == CHUNKED) {
 		if (chunkBuffer.find(HTTP_BODY_START) != std::string::npos && !chunkHeadSent)
		{
			size_t csize = chunkBuffer.find(HTTP_BODY_START) + HTTP_BODY_START_OFFSET;
			chunk += chunkBuffer.substr(0, csize);
			chunkBuffer = chunkBuffer.substr(chunk.size(), chunkBuffer.size());
			sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + csize);
			chunkHeadSent = true;
		}
		if (chunkBuffer.find(CRLF) != std::string::npos) {
			size_t csize = chunkBuffer.find(CRLF) + CRLF_OFFSET;
			csize += Utils::strHexToUint(chunkBuffer.substr(0, csize).c_str()) + CRLF_OFFSET;
			chunk += chunkBuffer.substr(0, csize);
			chunkBuffer = chunkBuffer.substr(csize, chunkBuffer.size());
			sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + csize);
		}	
		if (sendBuffer.empty()) {
			responseMode = NORMAL;
			chunkHeadSent = false;
			sendBuffer.clear();
		}
		if (send(socketFd, chunk.c_str(), chunk.size(), 0) < 1) {
			std::cerr << RED << "Send error: Server connection error" << END << std::endl;
			ConnectionManager::deleteConnection(this);
		}
		return ;
	}
	else if (send(socketFd, &sendBuffer[0], sendBuffer.size(), 0) < 1) {
		std::cerr << RED << "Send error: Server connection error" << END << std::endl;
		ConnectionManager::deleteConnection(this);
	}
	if (isOverPayloadLimit)
		ConnectionManager::deleteConnection(this);
	sendBuffer.clear();
}