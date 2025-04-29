#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include <cstring>
#include "FileManager.hpp"
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "ServerConstants.hpp"
#include "DataAdapter.hpp"
#include "Server.hpp"

std::list<Connection *> ConnectionManager::_connectionList;

ConnectionManager::ConnectionManager() {}

ConnectionManager::~ConnectionManager() {
	for (std::list<Connection *>::iterator connection = _connectionList.begin()
			; connection != _connectionList.end(); ++connection) {
		if (*connection)
				delete *connection;
	}
}

ConnectionManager::ConnectionManager(const ConnectionManager& src) {
	_connectionList = src._connectionList;
	_socketList = src._socketList;
}

ConnectionManager& ConnectionManager::operator=(const ConnectionManager& src) {
	if (this != &src) {
		_connectionList = src._connectionList;
		_socketList = src._socketList;
	}
	return *this;
}

int	ConnectionManager::pollSockets() {
	int		pollStatus;
	size_t	i = 0;
	size_t	arraySize = _socketList.size();
	pollfd	pollArray[arraySize];

	for (std::list<Socket>::iterator socket = _socketList.begin(); socket != _socketList.end(); ++socket)
		pollArray[i++] = socket->getPollFd();

	pollStatus = poll(pollArray, arraySize, 0);

	i = 0;
	for (std::list<Socket>::iterator socket = _socketList.begin(); socket != _socketList.end(); ++socket)
		socket->setPollFd(pollArray[i++]);
	
	return (pollStatus);
}

void	ConnectionManager::monitorConnections() {
	int	pollStatus;
	
	while (true) {
			pollStatus = pollSockets();
			if (pollStatus == -1) {
				std::cerr << RED << "Error: socket error" << END << std::endl;
			}
			else if (pollStatus > 0) {
				acceptConnections();
				std::list<Connection *>	cachedList(_connectionList);
				for (std::list<Connection *>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
					Connection&	connection = *(*it);
					Socket&		socket = connection.getSocket();


					if (socket.hasPollOut() && !connection.sendBuffer.empty())
						connection.sendData();
					else if (socket.hasPollIn() && !connection.isOverPayloadLimit)
						connection.recieveData();
					else if (connection.hasPendingCgi)
						connection.fetch();
					else if (socket.hasPollErr() || checkClientTimeOut(connection))
						deleteConnection(&connection);
				}
				cachedList.clear();
			}
	}
}


void	ConnectionManager::addListenSocket(Server& server) {
	int					optionEnable = 1;
	int					optionBufferSize = SOCKET_BUFFER_SIZE;
	struct sockaddr_in	address;
	struct pollfd		pollFd;

	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0) {
		std::cerr << RED << "Error: opening socket" << END << std::endl;
		exit(1);
	}

	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(socketFd, SOL_SOCKET, SO_RCVBUF, &optionBufferSize, sizeof(int)) < 0
	&&  setsockopt(socketFd, SOL_SOCKET, SO_SNDBUF, &optionBufferSize, sizeof(int)) < 0) {
		std::cerr << RED << "Error: setting socket options" << END << std::endl;
		close(socketFd);
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(server.getPort());

	if (bind(socketFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		if (errno == EADDRINUSE) {
			std::cout << BLUE << "Info: Port " << server.getPort() << " already in use" << END << " 🚀" << std::endl;
			std::cout << GREEN << "Virtual Server " << server.getName() << " listening on port: " << server.getPort() <<  END << " 🚀" << std::endl;
			return ;
		}
		else {
			std::cerr << RED << "Error: failed to bind address" << END << std::endl;
			close(socketFd);
			exit(1);
		}
	}
	
	if (listen(socketFd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
		std::cerr << RED << "Error: socket listen failed" << END << std::endl;
		close(socketFd);
		exit(1);
	}
	std::cout << GREEN << "Server " << server.getName() << " listening on port: " << server.getPort() <<  END << " 🚀" << std::endl;

	pollFd = pollfd();
	pollFd.fd = socketFd;
	pollFd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	pollFd.revents = 0;

	Socket newSocket(server.getPort(),pollFd, LISTEN);
	_socketList.push_back(newSocket);
}

void	ConnectionManager::acceptConnections() {
	std::list<Socket> cachedSocketList(_socketList);

	for (std::list<Socket>::iterator socket = cachedSocketList.begin(); socket != cachedSocketList.end(); ++socket) {
		if (socket->getType() == LISTEN && socket->hasPollIn()) {
			struct pollfd pollFd = pollfd();
			sockaddr_in	client_addr;
			socklen_t	client_addr_len = sizeof(client_addr);

			pollFd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
			pollFd.revents = 0;
			pollFd.fd  = accept(socket->getPollFd().fd, (sockaddr *)&client_addr, &client_addr_len);
			if (pollFd.fd == -1)
				std::cerr << RED << "Error: client connection error " << END << std::endl;
			
			Socket newSocket(socket->getPort(), pollFd, CONNECTION);
			_socketList.push_back(newSocket);
			_connectionList.push_back(new Connection(*(std::find(_socketList.begin(), _socketList.end(), newSocket))));
		}
	}
}

void	ConnectionManager::addConnection(Connection *connection) {
	_connectionList.push_back(connection);
}
void	ConnectionManager::deleteConnection(Connection *connection) {
	_connectionList.erase(std::find(_connectionList.begin(),_connectionList.end(), connection));
	delete(connection);
}

bool	ConnectionManager::checkClientTimeOut(Connection& connection) {
	time_t	actualTime, timeOut, lastTime;
	
	if (!connection.hasServerAssigned)
		return false;
	
	actualTime =  time(NULL);
	timeOut = connection.getServer().getClientTimeOut();
	lastTime = connection.getLastTime();

	if ((actualTime - lastTime) > timeOut && !connection.hasPendingCgi) {
		std::cout << BLUE << "Info: Server '" << connection.getServer().getName()
		<< "' closed connection " << connection.getSocket().getPollFd().fd 
		<< " (client timeout)" << END << std::endl;
		return true;
	}
	return false;
}