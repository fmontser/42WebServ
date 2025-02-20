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

ConnectionManager::ConnectionManager() {}
ConnectionManager::~ConnectionManager() {}

ConnectionManager::ConnectionManager(const ConnectionManager& src) { (void)src; }

ConnectionManager& ConnectionManager::operator=(const ConnectionManager& src) {
	if (this != &src) {
		(void)src;
	}
	return *this;
}

static int	pollSockets(Server& server) {
	int						pollStatus;
	std::list<Connection *>	connectionList;
	size_t					i = 1;

	connectionList = server.getConnectionList();
	pollfd	pollArray[connectionList.size() + 1];
	pollArray[0] = server.getPollfd();
	for (std::list<Connection *>::iterator it = connectionList.begin(); it != connectionList.end(); ++it)
		pollArray[i++] = (*it)->getPollFd();
	pollStatus = poll(pollArray, connectionList.size() + 1, 0);

	i = 1;
	server.setPollfd(pollArray[0]);
	for (std::list<Connection *>::iterator it = connectionList.begin(); it != connectionList.end(); ++it)
		(*it)->updatePollFd(pollArray[i++]);
	return (pollStatus);
}

static void	manageServerSocket(Server& server) {
	if (server.hasPollIn()) {
		Connection	*newConnection = new Connection(server);
		ConnectionManager::addConnection(server, newConnection);
	}
}

static void	manageServerConnections(Server& server) {
	std::list<Connection *>	cachedList(server.getConnectionList());
	for (std::list<Connection *>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
		Connection&	connection = *(*it);
		if (connection.hasPollIn())
			connection.recieveData();
		else if (connection.hasPollOut())
			connection.sendData();
	}
	cachedList.clear();
}

void	ConnectionManager::monitorConnections() {
	int	pollStatus;
	
	while (true) {
		for (std::map<std::string, Server>::iterator it = Config::getServers().begin();
			it != Config::getServers().end(); ++it) {
			Server&	server = it->second;
			pollStatus = pollSockets(server);
			if (pollStatus == -1) {
				std::cerr << RED << "Server error: Server connection error" << END << std::endl;
			}
			else if (pollStatus > 0) {
				manageServerSocket(server);
				manageServerConnections(server);
			}
		}
	}
}

void	ConnectionManager::addConnection(Server& server, Connection *connection) {
	server.getConnectionList().push_back(connection);
}

void	ConnectionManager::deleteConnection(Server& server, Connection *connection) {
	server.getConnectionList().erase(std::find(server.getConnectionList().begin(),server.getConnectionList().end(), connection));
	delete(connection);
}

