#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include <cstring>
#include "Config.hpp"
#include "SocketManager.hpp"
#include "ServerConstants.hpp"
#include "DataAdapter.hpp"



static int pollSockets(std::list<Socket *> &socketList) {
		int		pollStatus;
		pollfd	pollArray[socketList.size()];
		size_t	i = 0;
		for (std::list<Socket *>::iterator it = socketList.begin(); it != socketList.end(); ++it)
			pollArray[i++] = (*it)->getPollFd();
		pollStatus = poll(pollArray, socketList.size(), 0);
		
		i = 0;
		for (std::list<Socket *>::iterator it = socketList.begin(); it != socketList.end(); ++it)
			(*it)->updatePollFd(pollArray[i++]);
		return (pollStatus);
}

static bool	acceptConnection(Server& server, Socket *serverSocket) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);
	bool	asClient = false;

	int newSocketFd = accept(serverSocket->getFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
		return (false);
	}
	Socket *newSocket = new Socket();
	newSocket->setPort(serverSocket->getPort());
	newSocket->setFd(newSocketFd);
	newSocket->setParentServer(&server);
	newSocket->enableSocket(asClient);
	SocketManager::addSocket(server, newSocket);
	return (true);
}

SocketManager::SocketManager() {}
SocketManager::~SocketManager() {}

SocketManager::SocketManager(const SocketManager& src) { (void)src; }

SocketManager& SocketManager::operator=(const SocketManager& src) {
	if (this != &src) {
		(void)src;
	}
	return *this;
}

void	SocketManager::recieveData(Socket *socket) {
	char		buffer[READ_BUFFER] = {0};
	std::string	requestData;

	int len = recv(socket->getFd(), buffer, READ_BUFFER, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << socket->getFd() << END << std::endl;
	}
	else if (len > 0) {
		requestData.clear();
		requestData.append(buffer);
		DataAdapter::recieveData(socket, requestData);
	}
}

void	SocketManager::monitorSockets() {
	int		pollStatus;
	
	while (true) {
		for (std::map<std::string, Server>::iterator servIt = Config::getServers().begin();
			servIt != Config::getServers().end(); ++servIt) {
			pollStatus = pollSockets(servIt->second.getSocketList());
			if (pollStatus == -1) {
				//TODO gestionar error de socket ... error 500??
				std::cerr << RED << "Server error: Server socket error" << END << std::endl;
			}
			else if (pollStatus > 0) {
				std::list<Socket *> cachedList(servIt->second.getSocketList());
				for (std::list<Socket *>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
					if ((*it)->getPollFd().revents & POLLIN) {
						if ((*it)->getServerFlag() && acceptConnection(servIt->second, *it))
							recieveData(servIt->second.getSocketList().back());
						else
							recieveData(*it);
					}
					else if ((*it)->getPollFd().revents & POLLOUT) {
						if ((*it)->sendBuffer.size() > 0)
							sendData(*it);
					}
					else if ((*it)->getPollFd().revents & POLLHUP) {
						//TODO NO FUNCIONA??

					}
					else if ((*it)->getPollFd().revents & POLLERR) {
						//TODO NO FUNCIONA??
					}
				}
				cachedList.clear();
			}
		}
	}
}

void	SocketManager::recieveResponse(Socket *socket, const std::string& response, bool hasChunks) {
	socket->setChunkMode(hasChunks);
	socket->sendBuffer = response;
}

void	SocketManager::sendData(Socket *socket) {
	std::string chunk;
	static bool chunkHeadSent = false;

	chunk.clear();
	if (socket->getChunkMode()) {
		if (socket->sendBuffer.find(HTTP_BODY_START) != std::string::npos && !chunkHeadSent)
		{
			size_t csize = socket->sendBuffer.find(HTTP_BODY_START) + 4;
			chunk += socket->sendBuffer.substr(0, csize);
			socket->sendBuffer = socket->sendBuffer.substr(chunk.size(), socket->sendBuffer.size());
			chunkHeadSent = true;
		}
		if (socket->sendBuffer.find(CRLF) != std::string::npos) {
			size_t csize = socket->sendBuffer.find(CRLF) + 2;
			csize += std::strtol(socket->sendBuffer.substr(0, csize).c_str(), NULL, 16) + 2;
			chunk += socket->sendBuffer.substr(0, csize);
			socket->sendBuffer = socket->sendBuffer.substr(csize, socket->sendBuffer.size());
		}	
		if (socket->sendBuffer.empty()) {
			socket->setChunkMode(false);
			chunkHeadSent = false;
		}
		if (send(socket->getFd(), chunk.c_str(), chunk.size(), 0) == -1) {
			std::cerr << RED << "Send error: Server socket error" << END << std::endl;
		}
		return ;
	}
	else if (send(socket->getFd(), socket->sendBuffer.c_str(), socket->sendBuffer.size(), 0) == -1) {
		std::cerr << RED << "Send error: Server socket error" << END << std::endl;
	}
	socket->sendBuffer.clear();
}

void	SocketManager::addSocket(Server& server, Socket *socket) {
	server.getSocketList().push_back(socket);
}

void	SocketManager::deleteSocket(Server& server,Socket *socket) {
	close(socket->getFd());
	server.getSocketList().erase(std::find(server.getSocketList().begin(),server.getSocketList().end(), socket));
	//TODO comprobar que la eliminacion es correcta tras finalizar una conexion
}



/*
	Event types that can be polled for.  These bits may be set in `events'
	to indicate the interesting event types; they will appear in `revents'
	to indicate the status of the file descriptor.  
	#define POLLIN		0x001		 There is data to read.
	#define POLLPRI		0x002		 There is urgent data to read.
	#define POLLOUT		0x004		 Writing now will not block.
	#if defined __USE_XOPEN || defined __USE_XOPEN2K8
	 These values are defined in XPG4.2.
	# define POLLRDNORM	0x040		 Normal data may be read.
	# define POLLRDBAND	0x080		 Priority data may be read.
	# define POLLWRNORM	0x100		 Writing now will not block.
	# define POLLWRBAND	0x200		 Priority data may be written.
	#endif
	#ifdef __USE_GNU
	 These are extensions for Linux.
	# define POLLMSG	0x400
	# define POLLREMOVE	0x1000
	# define POLLRDHUP	0x2000
	#endif
	 Event types always implicitly polled for.  These bits need not be set in
	`events', but they will appear in `revents' to indicate the status of
	the file descriptor.
	#define POLLERR		0x008		 Error condition.
	#define POLLHUP		0x010		 Hung up.
	#define POLLNVAL	0x020		 Invalid polling request.
 */