#pragma once

#include <string>
#include "Socket.hpp"
#include "Server.hpp"

class SocketManager {
private:
	SocketManager();
	~SocketManager();
	SocketManager(const SocketManager& src);
	SocketManager& operator=(const SocketManager& src);

	static void	recieveData(Socket& socket);
	static void	sendData(Socket& socket);

public:

	static void	monitorSockets();
	static void	recieveResponse(Socket& socket, const std::string& response);
	static void	addSocket(Server& server, Socket& socket);
	static void	deleteSocket(Server& server, Socket& socket);
};