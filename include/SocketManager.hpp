#pragma once

#include <string>
#include <list>
#include "Socket.hpp"

class SocketManager {
private:
	std::list<Socket&>	_socketList;
	std::string			buffer;

	SocketManager(const SocketManager& src);
	SocketManager& operator=(const SocketManager& src);

	void	recieveData();

public:
	SocketManager();
	~SocketManager();

	void	monitorSocket();
	void	sendData();
	void	addSocket(Socket& socket);
	void	deleteSocket(Socket& socket);
};