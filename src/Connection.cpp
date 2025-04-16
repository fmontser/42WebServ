#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "DataAdapter.hpp"
#include "HttpProcessor.hpp"
#include "Utils.hpp"

Connection::Connection(Server& server) : _server(server), _multiDataAdapter(NULL), _multiCgiAdapter(NULL) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	_socketFd = accept(_server.getSocketFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (_socketFd == -1) {
		std::cerr << RED << "Error: client connection error " << END << std::endl;
	}

	isOverPayloadLimit = false;
	hasPendingCgi = false;
	requestMode = SINGLE;
	responseMode = NORMAL;
	contentLength = 0;
	recvBuffer.clear();
	sendBuffer.clear();
	boundarie.clear();

	_pollfd = pollfd();
	_pollfd.fd = _socketFd;
	_pollfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_pollfd.revents = 0;
}

Connection::Connection(const Connection& src) : _server(src._server) {
	_socketFd = src._socketFd;
	_pollfd = src._pollfd;
	recvBuffer = src.recvBuffer;
	sendBuffer = src.sendBuffer;
	isOverPayloadLimit = src.isOverPayloadLimit;
	hasPendingCgi = src.hasPendingCgi;
	responseMode = src.responseMode;
	requestMode = src.requestMode;
	boundarie = src.boundarie;
	contentLength = src.contentLength;
}

Connection& Connection::operator=(const Connection& src) {
	if (this != &src) {
		_socketFd = src._socketFd;
		_pollfd = src._pollfd;
		recvBuffer = src.recvBuffer;
		sendBuffer = src.sendBuffer;
		isOverPayloadLimit = src.isOverPayloadLimit;
		hasPendingCgi = src.hasPendingCgi;
		responseMode = src.responseMode;
		requestMode = src.requestMode;
		boundarie = src.boundarie;
		contentLength = src.contentLength;
	}
	return *this;
}

Connection::~Connection() {
	close(_socketFd);
}

Server&			Connection::getServer() const { return _server; }
struct pollfd	Connection::getPollFd() const { return _pollfd; }

static void	checkBinaryDownload(HttpRequest& request) {
	size_t downloadParamPos = request.url.find("?download=true");
	if (downloadParamPos != std::string::npos) {
		request.url = request.url.substr(0, downloadParamPos);
		request.isBinaryDownload =  true;
	}
}

void	Connection::resetConnection() {
	delete _multiDataAdapter;
	_multiDataAdapter = NULL;
	delete _multiCgiAdapter;
	_multiCgiAdapter = NULL;

	boundarie.clear();
	boundStart.clear();
	boundEnd.clear();
	requestMode = Connection::SINGLE;
}

void	Connection::manageSingle(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter){
	dataAdapter.deserializeRequest();

	std::cout	<< BLUE << "Fd: " << dataAdapter.getConnection()->getPollFd().fd
				<< " requested: " << dataAdapter.getRequest().url << END << std::endl;

	dataAdapter.getRequest().isCgiRequest 
		= CgiAdapter::isCgiRequest(dataAdapter.getRequest().url);
	checkBinaryDownload(dataAdapter.getRequest());

	HttpProcessor::processHttpRequest(dataAdapter, cgiAdapter);

	dataAdapter.serializeResponse();	
	if (requestMode == Connection::MULTIPART) {
		_multiDataAdapter = new DataAdapter(dataAdapter); //TODO podria estar reservandose encima del anterior si es cgi??? leak!!
		_multiDataAdapter->getResponse().statusCode = "";
		_multiCgiAdapter = new CgiAdapter();
	}

	//TODO cuidado!!!! comprobar que es correcto
	if (!hasPendingCgi)
		resetConnection();


	recvBuffer.clear();
}

void	Connection::manageMultiPart(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter){
	dataAdapter.deserializeRequest();
	dataAdapter.getRequest().method = "POST";
	dataAdapter.getRequest().isCgiRequest 
		= CgiAdapter::isCgiRequest(dataAdapter.getRequest().url);
	checkBinaryDownload(dataAdapter.getRequest());

	HttpProcessor::processHttpRequest(dataAdapter, cgiAdapter);
	
	if (!dataAdapter.getResponse().statusCode.empty())
		dataAdapter.serializeResponse();
	recvBuffer.clear();
	dataAdapter.getRequest().body.clear();
	 if (contentLength == 0)
		resetConnection();
}

void	Connection::fetchCgi() {
	if (requestMode == MULTIPART)
		manageMultiPart(*_multiDataAdapter, *_multiCgiAdapter);
	else 
		manageSingle(*_multiDataAdapter, *_multiCgiAdapter);
}

void	Connection::recieveData() {

	//TODO @@@@1111 adapters al HEAP siempre!!! refactor.



	DataAdapter	dataAdapter = DataAdapter(this);
	CgiAdapter	cgiAdapter;
	char		buffer[READ_BUFFER] = {0};
	int			len;

	len = recv(_socketFd, buffer, READ_BUFFER, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << _socketFd << END << std::endl;
		ConnectionManager::deleteConnection(_server, this);
	}
	else if (len == 0) {
		std::cout << BLUE << "Info: client closed connection " << _socketFd << END << std::endl;
		ConnectionManager::deleteConnection(_server, this);
	}
	else if (len > 0) {
		recvBuffer.assign(buffer, buffer + len);
		if (requestMode == MULTIPART) {
			contentLength -= len;
			manageMultiPart(*_multiDataAdapter, *_multiCgiAdapter);
		}
		else 
			manageSingle(dataAdapter,cgiAdapter);
	}
}

void	Connection::sendData() {
	std::string chunk;
	static bool chunkHeadSent = false;
	std::string _chunkBuffer = std::string(sendBuffer.begin(), sendBuffer.end());

	chunk.clear();
	if (responseMode == CHUNKED) {
 		if (_chunkBuffer.find(HTTP_BODY_START) != std::string::npos && !chunkHeadSent)
		{
			size_t csize = _chunkBuffer.find(HTTP_BODY_START) + HTTP_BODY_START_OFFSET;
			chunk += _chunkBuffer.substr(0, csize);
			_chunkBuffer = _chunkBuffer.substr(chunk.size(), _chunkBuffer.size());
			sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + csize);
			chunkHeadSent = true;
		}
		if (_chunkBuffer.find(CRLF) != std::string::npos) {
			size_t csize = _chunkBuffer.find(CRLF) + CRLF_OFFSET;
			csize += Utils::strHexToUint(_chunkBuffer.substr(0, csize).c_str()) + CRLF_OFFSET;
			chunk += _chunkBuffer.substr(0, csize);
			_chunkBuffer = _chunkBuffer.substr(csize, _chunkBuffer.size());
			sendBuffer.erase(sendBuffer.begin(), sendBuffer.begin() + csize);
		}	
		if (sendBuffer.empty()) {
			responseMode = NORMAL;
			chunkHeadSent = false;
			sendBuffer.clear();
		}
		if (send(_socketFd, chunk.c_str(), chunk.size(), 0) < 1) {
			std::cerr << RED << "Send error: Server connection error" << END << std::endl;
			ConnectionManager::deleteConnection(_server, this);
		}
		return ;
	}
	else if (send(_socketFd, &sendBuffer[0], sendBuffer.size(), 0) < 1) {
		std::cerr << RED << "Send error: Server connection error" << END << std::endl;
		ConnectionManager::deleteConnection(_server, this);
	}
	if (isOverPayloadLimit)
		ConnectionManager::deleteConnection(_server, this);
	sendBuffer.clear();
}

void			Connection::dinamizeAdapters(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter){
	_multiDataAdapter = new DataAdapter(dataAdapter);
	_multiCgiAdapter = new CgiAdapter(cgiAdapter);
}


void	Connection::updatePollFd(struct pollfd pfd) { _pollfd = pfd; }
bool	Connection::hasPollErr() const { return _pollfd.revents & POLLERR; }
bool	Connection::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool	Connection::hasPollOut() const { return _pollfd.revents & POLLOUT; }

