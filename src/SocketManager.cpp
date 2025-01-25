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

std::list<Socket *>	SocketManager::_socketList;
int					SocketManager::_activeFd;

static int	acceptConnection(Socket& serverSocket) {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	int newSocketFd = accept(serverSocket.getFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
	}
	return newSocketFd;
}

SocketManager::SocketManager() {}
SocketManager::~SocketManager() {
	for (std::list<Socket *>::iterator it = _socketList.begin(); it != _socketList.end(); ++it)
		deleteSocket(*(*it));
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
	bool	asClient = false;

	while (true) {

		//TODO @@@@@ megacagada... la informacion se pierde....ref/stack/puntero....al heap?
		size_t arraySize = 0;
		pollfd pollArray[_socketList.size()];
		for (std::list<Socket *>::iterator it = _socketList.begin(); it != _socketList.end(); ++it) {
			pollArray[arraySize++] = (*it)->getPollFd();
		}
		pollStatus = poll(pollArray, _socketList.size(), TIMEOUT);
		if (pollStatus == -1) {
			//TODO gestionar error de socket
			std::cerr << RED << "Server error: Server socket error" << END << std::endl;
		}
		else if (pollStatus > 0) {
			for (std::list<Socket *>::iterator it = _socketList.begin(); it != _socketList.end(); ++it) {
				_activeFd = (*it)->getFd();
				if ((*it)->getPollFd().revents & POLLIN) {
					if ((*it)->getServerFlag()) {
						Socket newSocket;
						newSocket.setPort(acceptConnection(*(*it)));
						newSocket.enableSocket(asClient);
						addSocket(newSocket);
						recieveData(newSocket);
					}
					else {
						recieveData(*(*it));
					}
				}
			}
		}
	}
}

void	SocketManager::sendResponse(const std::string& response) {
	if (send(_activeFd, response.c_str(), response.size(), 0) == -1) {
		std::cerr << RED << "Send error: Server socket error" << END << std::endl;
	}
}

void	SocketManager::addSocket(Socket& socket) {
	_socketList.push_back(&socket);
}

void	SocketManager::deleteSocket(Socket& socket) {
	close(socket.getFd());
	_socketList.erase(std::find(_socketList.begin(),_socketList.end(), &socket)); //TODO check...
}