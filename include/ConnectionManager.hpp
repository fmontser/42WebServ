#pragma once

#include <string>
#include "Connection.hpp"
#include "Server.hpp"

class ConnectionManager {
private:
	ConnectionManager();
	~ConnectionManager();
	ConnectionManager(const ConnectionManager& src);
	ConnectionManager& operator=(const ConnectionManager& src);

	static void	recieveData(Connection *connection);
	static void	sendData(Connection *connection);

public:

	static void	monitorConnections();
	static void	recieveResponse(Connection *connection, const std::string& response, bool hasChunks);
	static void	addConnection(Server& server, Connection *connection);
	static void	deleteConnection(Connection *connection);
};