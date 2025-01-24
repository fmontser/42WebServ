#pragma once

#include <string>
#include <list>
#include "Socket.hpp"

class SocketManager {
private:
	std::list<Socket&>	_socketList;
	std::string			buffer;

	void	recieveData();
public:
	SocketManager();
	~SocketManager();
	SocketManager(const SocketManager& src);
	SocketManager& operator=(const SocketManager& src);

	void	monitorSocket();
	void	sendData(const std::string& response);
	void	addSocket(Socket& socket);
	void	deleteSocket(Socket& socket);
};