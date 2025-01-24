#pragma once

#include <string>
#include <list>
#include "Socket.hpp"

class SocketManager {
private:
	static std::list<Socket>	_socketList;
	static std::string			_buffer;

	SocketManager();
	~SocketManager();
	SocketManager(const SocketManager& src);
	SocketManager& operator=(const SocketManager& src);

	static void	recieveData();
public:
	static void	monitorSockets();
	static void	sendData(const std::string& response);
	static void	addSocket(Socket& socket);
	static void	deleteSocket(Socket& socket);
};