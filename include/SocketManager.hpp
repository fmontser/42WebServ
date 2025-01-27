#pragma once

#include <string>
#include <list>
#include "Socket.hpp"

class SocketManager {
private:
	static std::list<Socket>	_socketList;
	static int					_activeFd;

	SocketManager();
	~SocketManager();
	SocketManager(const SocketManager& src);
	SocketManager& operator=(const SocketManager& src);

	static void	recieveData(Socket& socket);
public:

	static void	monitorSockets();
	static void	sendResponse(const std::string& response);
	static void	addSocket(Socket& socket);
	static void	deleteSocket(Socket& socket);
};