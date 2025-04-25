#pragma once

#include <string>
#include "Connection.hpp"
#include "Server.hpp"
#include "Socket.hpp"

class ConnectionManager {
	private:

		static std::list<Connection *>	_connectionList;
		std::list<Socket>		_socketList;;
		
		void		addListenSocket(Server& server);
		int			pollSockets();

		void		acceptConnections();
		void		addConnection(Connection *connection);
		
		void		recieveData(Connection *connection);
		void		sendData(Connection *connection);
		
	public:
		
		ConnectionManager();
		~ConnectionManager();
		ConnectionManager(const ConnectionManager& src);
		ConnectionManager& operator=(const ConnectionManager& src);
		
		void	monitorConnections();
		static void	deleteConnection(Connection *connection);
	};