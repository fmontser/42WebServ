#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <algorithm>
#include "SocketManager.hpp"
#include "ServerConstants.hpp"
#include "Socket.hpp"
#include "DataAdapter.hpp"

#include <cstring>

std::list<Socket>	SocketManager::_socketList;
int					SocketManager::_activeFd;

static int pollSockets(std::list<Socket> &socketList) {
		int		pollStatus;
		pollfd	pollArray[socketList.size()];
		size_t	i = 0;
		for (std::list<Socket>::iterator it = socketList.begin(); it != socketList.end(); ++it)
			pollArray[i++] = (*it).getPollFd();
		pollStatus = poll(pollArray, socketList.size(), TIMEOUT);
		
		i = 0;
		for (std::list<Socket>::iterator it = socketList.begin(); it != socketList.end(); ++it)
			(*it).updatePollFd(pollArray[i++]);
		return (pollStatus);
}

static void	acceptConnection(Socket& serverSocket) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);
	bool	asClient = false;

	int newSocketFd = accept(serverSocket.getFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
		//TODO manage error?
		return ;
	}
	Socket newSocket;
	newSocket.setPort(serverSocket.getPort());
	newSocket.setFd(newSocketFd);
	newSocket.enableSocket(asClient);
	SocketManager::addSocket(newSocket);
}

SocketManager::SocketManager() {}
SocketManager::~SocketManager() {
	for (std::list<Socket>::iterator it = _socketList.begin(); it != _socketList.end(); ++it)
		deleteSocket(*it);
}

SocketManager::SocketManager(const SocketManager& src) {
	_socketList = src._socketList;
	_activeFd = src._activeFd;
}

SocketManager& SocketManager::operator=(const SocketManager& src) {
	if (this != &src) {
		_socketList = src._socketList;
		_activeFd = src._activeFd;
	}
	return *this;
}

void	SocketManager::recieveData(Socket& socket) {
	char		buffer[BUFFER_SIZE] = {0};
	std::string	request;

	int len = recv(socket.getFd(), buffer, BUFFER_SIZE, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << socket.getFd() << END << std::endl;
	}
	else if (len > 0) {
		request.clear();
		request.append(buffer);
		DataAdapter::recieveData(request);
	}
}

void	SocketManager::monitorSockets() {
	int		pollStatus;
	
	while (true) {
		pollStatus = pollSockets(_socketList);
		if (pollStatus == -1) {
			//TODO gestionar error de socket
			std::cerr << RED << "Server error: Server socket error" << END << std::endl;
		}
		else if (pollStatus > 0) {
			std::list<Socket> cachedList(_socketList);
			for (std::list<Socket>::iterator it = cachedList.begin(); it != cachedList.end(); ++it) {
				_activeFd = (*it).getFd();
				if ((*it).getPollFd().revents & POLLIN) {
					if ((*it).getServerFlag()) {
						acceptConnection(*it);
						_activeFd = _socketList.back().getFd();
						recieveData(_socketList.back());
					}
					else {
						recieveData(*it);
					}
				}
			}
			cachedList.clear();
		}
	}
}

void	SocketManager::sendResponse(const std::string& response) {
	std::string r =  response;
	int	size = response.size();

	if (send(_activeFd, response.c_str(), size, 0) == -1) {
		std::cerr << RED << "Send error: Server socket error" << END << std::endl;
	}
}

void	SocketManager::addSocket(Socket& socket) {
	_socketList.push_back(socket);
}

void	SocketManager::deleteSocket(Socket& socket) {
	close(socket.getFd());
	_socketList.erase(std::find(_socketList.begin(),_socketList.end(), socket)); //TODO check...
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