#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "DataAdapter.hpp"
#include "RequestProcessor.hpp"
#include "Utils.hpp"

Connection::Connection(Server& server) : _server(server) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	_socketFd = accept(_server.getSocketFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (_socketFd == -1) {
		std::cerr << RED << "Error: client connection error " << END << std::endl;
	}

	isChunkedResponse = false;
	isMultipartUpload = false;
	boundarie.clear();
	contentLength = 0;

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
	isChunkedResponse = src.isChunkedResponse;
	isMultipartUpload = src.isMultipartUpload;
	boundarie = src.boundarie;
	contentLength = src.contentLength;
}

Connection& Connection::operator=(const Connection& src) {
	if (this != &src) {
		_socketFd = src._socketFd;
		_pollfd = src._pollfd;
		recvBuffer = src.recvBuffer;
		sendBuffer = src.sendBuffer;
		isChunkedResponse = src.isChunkedResponse;
		isMultipartUpload = src.isMultipartUpload;
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

void	Connection::recieveData() {

	//TODO multipart mode!!!


	DataAdapter adapter = DataAdapter(this);
	char		buffer[READ_BUFFER] = {0};
	int			len;

	len = recv(_socketFd, buffer, READ_BUFFER, 0);
	if (len == -1)
		std::cerr << RED << "Error: client error " << _socketFd << END << std::endl;
	else if (len == 0) {
		std::cout << BLUE << "Info: client closed connection " << _socketFd << END << std::endl;
		ConnectionManager::deleteConnection(_server, this);
	}
	else if (len > 0) {
		recvBuffer.clear();
		recvBuffer.append(buffer);
		adapter.deserializeRequest();
		HttpProcessor::processHttpRequest(adapter);
		adapter.serializeResponse();
	}
}

void	Connection::sendData() {
	std::string chunk;
	static bool chunkHeadSent = false;

	chunk.clear();
	if (isChunkedResponse) {
		if (sendBuffer.find(HTTP_BODY_START) != std::string::npos && !chunkHeadSent)
		{
			size_t csize = sendBuffer.find(HTTP_BODY_START) + HTTP_BODY_START_OFFSET;
			chunk += sendBuffer.substr(0, csize);
			sendBuffer = sendBuffer.substr(chunk.size(), sendBuffer.size());
			chunkHeadSent = true;
		}
		if (sendBuffer.find(CRLF) != std::string::npos) {
			size_t csize = sendBuffer.find(CRLF) + CRLF_OFFSET;
			csize += Utils::strHexToUint(sendBuffer.substr(0, csize).c_str()) + CRLF_OFFSET;
			chunk += sendBuffer.substr(0, csize);
			sendBuffer = sendBuffer.substr(csize, sendBuffer.size());
		}	
		if (sendBuffer.empty()) {
			isChunkedResponse = false;
			chunkHeadSent = false;
			sendBuffer.clear();
			recvBuffer.clear();
		}
		if (send(_socketFd, chunk.c_str(), chunk.size(), 0) == -1) {
			std::cerr << RED << "Send error: Server connection error" << END << std::endl;
		}
		return ;
	}
	else if (send(_socketFd, sendBuffer.c_str(), sendBuffer.size(), 0) == -1) {
		std::cerr << RED << "Send error: Server connection error" << END << std::endl;
	}
	sendBuffer.clear();
	recvBuffer.clear();
}

void	Connection::updatePollFd(struct pollfd pfd) { _pollfd = pfd; }
bool	Connection::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool	Connection::hasPollOut() const { return _pollfd.revents && POLLOUT; }

