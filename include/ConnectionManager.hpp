#pragma once

#include <string>
#include "Connection.hpp"
#include "Server.hpp"

class ConnectionManager {
	private:

		std::list<Connection *>	_connectionList;

		
		void	recieveData(Connection *connection);
		void	sendData(Connection *connection);
		
	public:
		
		ConnectionManager();
		~ConnectionManager();
		ConnectionManager(const ConnectionManager& src);
		ConnectionManager& operator=(const ConnectionManager& src);

		std::list<Connection *>&	getConnectionList();
		
		void	listenSocket(Server& server);
		void	monitorConnections();
		void	recieveResponse(Connection *connection, const std::string& response, bool hasChunks);
		void	addConnection(Connection *connection);
		void	deleteConnection(Connection *connection);
};