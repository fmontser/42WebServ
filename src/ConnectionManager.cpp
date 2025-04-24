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

static std::list<Connection *>	_connectionList;

ConnectionManager::ConnectionManager() {}
ConnectionManager::~ConnectionManager() {}

ConnectionManager::ConnectionManager(const ConnectionManager& src) { (void)src; }

ConnectionManager& ConnectionManager::operator=(const ConnectionManager& src) {
	if (this != &src) {
		(void)src;
	}
	return *this;
}


static int	pollSockets() {
	int						pollStatus;
	size_t					i = 0;

	pollfd	pollArray[_connectionList.size()];
	for (std::list<Connection *>::iterator it = _connectionList.begin(); it != _connectionList.end(); ++it)
		pollArray[i++] = (*it)->getPollFd();
	pollStatus = poll(pollArray, _connectionList.size(), 0);

	i = 0;
	for (std::list<Connection *>::iterator it = _connectionList.begin(); it != _connectionList.end(); ++it)
		(*it)->updatePollFd(pollArray[i++]);
	return (pollStatus);
}

static void	manageServerConnections() {


	if (server.hasPollIn()) {

		addConnection(connection);
	}

	//TODO @@@@@@@@@@@ disociar sockets de conexiones 


	std::list<Connection *>	cachedList(server.getConnectionList());
	for (std::list<Connection *>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
		Connection&	connection = *(*it);
		if (connection.hasPollErr())
			ConnectionManager::deleteConnection(server, &connection);
		else if (connection.hasPollOut() && !connection.sendBuffer.empty())
			connection.sendData();
		else if (connection.hasPollIn() && !connection.isOverPayloadLimit)
			connection.recieveData();
		else if (connection.hasPendingCgi || connection.isDerived)
			connection.fetch();

		if (connection.isDerived)
			break;
	}
	cachedList.clear();
}

void	ConnectionManager::monitorConnections() {
	int	pollStatus;
	
	while (true) {
			pollStatus = pollSockets();
			if (pollStatus == -1) {
				std::cerr << RED << "Error: socket error" << END << std::endl;
			}
			else if (pollStatus > 0) {
				manageServerConnections();
			}
		}
	}
}

void	ConnectionManager::listenSocket(Server& server) {
	int					optionEnable = 1;
	int					optionBufferSize = SOCKET_BUFFER_SIZE;
	struct sockaddr_in	address;

	int _socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0) {
		std::cerr << RED << "Error: opening socket" << END << std::endl;
		exit(1);
	}

	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_RCVBUF, &optionBufferSize, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_SNDBUF, &optionBufferSize, sizeof(int)) < 0) {
		std::cerr << RED << "Error: setting socket options" << END << std::endl;
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(server.getPort());

	if (bind(_socketFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		if (errno == EADDRINUSE)
			std::cerr << BLUE << "Warning: Port " << server.getPort() << " is already in use" << END << std::endl;
		else {
			std::cerr << RED << "Error: failed to bind address" << END << std::endl;
			exit(1);
		}
	}
	

	if (listen(_socketFd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
		std::cerr << RED << "Error: socket listen failed" << END << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Server " << server.getName() << " listening on port: " << server.getPort() <<  END << " 🚀" << std::endl;

	addConnection(new Connection(_socketFd));
}


void	ConnectionManager::addConnection(Connection *connection) {
	_connectionList.push_back(connection);
}

void	ConnectionManager::deleteConnection(Connection *connection) {
	_connectionList.erase(std::find(_connectionList.begin(),_connectionList.end(), connection));
	delete(connection);
}

std::list<Connection *>&			ConnectionManager::getConnectionList() { return _connectionList; }