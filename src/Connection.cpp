#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "DataAdapter.hpp"

Connection::Connection(Server& server) : _server(server) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	_socketFd = accept(_server.getSocketFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (_socketFd == -1) {
		std::cerr << RED << "Error: client connection error " << END << std::endl;
	}

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
	sendBufferSize = src.sendBufferSize;
}

Connection& Connection::operator=(const Connection& src) {
	if (this != &src) {
		_socketFd = src._socketFd;
		_pollfd = src._pollfd;
		recvBuffer = src.recvBuffer;
		sendBuffer = src.sendBuffer;
		sendBufferSize = src.sendBufferSize;
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
		adapter.processData();
	}
}

void	Connection::sendData() {

	//TODO borrar  envio completo temporal
	if (sendBufferSize > 0 && send(_socketFd, sendBuffer.c_str(), sendBufferSize, 0) == -1)
		std::cerr << RED << "Send error: Server connection error" << END << std::endl;
	sendBufferSize = 0;
	sendBuffer.clear();
}

void	Connection::updatePollFd(struct pollfd pfd) { _pollfd = pfd; }
bool	Connection::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool	Connection::hasPollOut() const { return _pollfd.revents && POLLOUT; }

/* 	std::string chunk;
	static bool chunkHeadSent = false;

	chunk.clear();
	if (connection->chunkMode) {
		if (connection->sendBuffer.find(HTTP_BODY_START) != std::string::npos && !chunkHeadSent)
		{
			size_t csize = connection->sendBuffer.find(HTTP_BODY_START) + 4;
			chunk += connection->sendBuffer.substr(0, csize);
			connection->sendBuffer = connection->sendBuffer.substr(chunk.size(), connection->sendBuffer.size());
			chunkHeadSent = true;
		}
		if (connection->sendBuffer.find(CRLF) != std::string::npos) {
			size_t csize = connection->sendBuffer.find(CRLF) + 2;
			csize += std::strtol(connection->sendBuffer.substr(0, csize).c_str(), NULL, 16) + 2;
			chunk += connection->sendBuffer.substr(0, csize);
			connection->sendBuffer = connection->sendBuffer.substr(csize, connection->sendBuffer.size());
		}	
		if (connection->sendBuffer.empty()) {
			connection->chunkMode = false;
			chunkHeadSent = false;
		}
		if (send(connection->getFd(), chunk.c_str(), chunk.size(), 0) == -1) {
			std::cerr << RED << "Send error: Server connection error" << END << std::endl;
		}
		return ;
	}
	else if (send(connection->getFd(), connection->sendBuffer.c_str(), connection->sendBuffer.size(), 0) == -1) {
		std::cerr << RED << "Send error: Server connection error" << END << std::endl;
	}
	connection->sendBuffer.clear(); */