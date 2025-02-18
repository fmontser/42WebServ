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

static int pollSockets(Server& server) {
		int		pollStatus;
		std::list<Connection *> connectionList;
		pollfd	pollArray[connectionList.size() + 1];
		size_t	i = 1;
		
		connectionList = server.getConnectionList();
		pollArray[0].fd = server.getSocketFd();
		for (std::list<Connection *>::iterator it = connectionList.begin(); it != connectionList.end(); ++it)
			pollArray[i++] = (*it)->getPollFd();
		pollStatus = poll(pollArray, connectionList.size(), 0);

		//TODO @@@@@@@@ el server no tiene pollfd o no seactualiza... 
		i = 1;
		for (std::list<Connection *>::iterator it = connectionList.begin(); it != connectionList.end(); ++it)
			(*it)->updatePollFd(pollArray[i++]);
		return (pollStatus);
}


void	ConnectionManager::monitorConnections() {
	int		pollStatus;
	
	while (true) {
		for (std::map<std::string, Server>::iterator servIt = Config::getServers().begin();
			servIt != Config::getServers().end(); ++servIt) {
			pollStatus = pollSockets(servIt->second);
			if (pollStatus == -1) {
				std::cerr << RED << "Server error: Server connection error" << END << std::endl;
			}
			else if (pollStatus > 0) {
				std::list<Connection *> cachedList(servIt->second.getConnectionList());
				for (std::list<Connection *>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
					if ((*it)->hasPollIn()) {
						//TODO recieve via connection
					}
					else if ((*it)->hasPollOut()) {
						//TODO send via connection
					}
/* 					else if ((*it)->getPollFd().revents & POLLHUP) {
						deleteConnection(*it);
						std::cout << BLUE <<"Info: Client on connection fd " << (*it)->getFd() << " hang up" << END << std::endl;
					}
					else if ((*it)->getPollFd().revents & POLLERR) {
						deleteConnection(*it);
						std::cerr << RED <<"Error: Client error on connection fd " << (*it)->getFd() << END << std::endl;
					} */
				}
				cachedList.clear();
			}
		}
	}
}



void	ConnectionManager::addConnection(Server& server, Connection *connection) {
	server.getConnectionList().push_back(connection);
}

void	ConnectionManager::deleteConnection(Connection *connection) {
	Server server = connection->getServer();
	server.getConnectionList().erase(std::find(server.getConnectionList().begin(),server.getConnectionList().end(), connection));
	delete(connection);
}

