#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include "Connection.hpp"
#include "ConnectionManager.hpp"



Connection::Connection(Server& server) : _server(server) {
	//TODO accept conection??
}

Connection::Connection(const Connection& src) : _server(src._server) {
	_socketFd =  src._socketFd;
	recvBuffer = src.recvBuffer;
	sendBuffer = src.sendBuffer;
	sendBufferSize = src.sendBufferSize;
}

Connection& Connection::operator=(const Connection& src) {
	if (this != &src) {
		_socketFd = src._socketFd;
		recvBuffer = src.recvBuffer;
		sendBuffer = src.sendBuffer;
		sendBufferSize = src.sendBufferSize;
	}
	return *this;
}

Connection::~Connection() {
	close(_socketFd);
}


void	Connection::acceptConnection() {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	int		hardcodedServerSocket = 3; //TODO cambiar server!

	_socketFd = accept(hardcodedServerSocket, (sockaddr *)&client_addr, &client_addr_len);
	if (_socketFd == -1)
		std::cerr << RED << "Error: client connection error " << END << std::endl;
	//TODO anadir a la lista de conexiones...
	ConnectionManager::addConnection(_server, this); //TODO esto esta mal

}

Server&	Connection::getServer() const { return _server; }

struct pollfd	Connection::getPollFd() const { return _pollfd; }
bool			Connection::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool			Connection::hasPollOut() const { return _pollfd.revents && POLLOUT; }


void			Connection::recieveData() {

}

void			Connection::sendData() {

}

void			Connection::updatePollFd(struct pollfd pfd) { _pollfd = pfd; }
/* 

void	ConnectionManager::recieveData(Connection *connection) {
	char		buffer[READ_BUFFER] = {0};
	std::string	requestData;


	int len = recv(connection->getFd(), buffer, READ_BUFFER, 0);

	if (connection->contentLength > 0) {
		connection->contentLength -= READ_BUFFER;
		if (connection->contentLength < 0)
			connection->contentLength = 0;
		connection->recvBuffer += buffer;
		if (connection->contentLength == 0) {
			FileManager::processMultiPart(connection);
			connection->recvBuffer.clear();
		}
		return ;
	}

	if (len == -1)
		std::cerr << RED << "Error: client error " << connection->getFd() << END << std::endl;
	else if (len == 0) {
		std::cout << BLUE << "Info: client closed connection " << connection->getFd() << END << std::endl;
		deleteConnection(connection);
	}
	else if (len > 0) {
		requestData.clear();
		requestData.append(buffer);
		DataAdapter::recieveData(connection, requestData);
	}
}



void	ConnectionManager::recieveResponse(Connection *connection, const std::string& response, bool hasChunks) {
	connection->chunkMode = hasChunks;
	connection->sendBuffer = response;
}

void	ConnectionManager::sendData(Connection *connection) {
	std::string chunk;
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
	connection->sendBuffer.clear();
}

 */