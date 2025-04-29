#pragma once

#include <string>
#include "Connection.hpp"
#include "Server.hpp"
#include "Socket.hpp"

class ConnectionManager {
	private:

		int			pollSockets();
		
		void		acceptConnections();
		void		addConnection(Connection *connection);

		bool		checkClientTimeOut(Connection& connection);
	
	public:
	
		ConnectionManager();
		~ConnectionManager();
		ConnectionManager(const ConnectionManager& src);
		ConnectionManager& operator=(const ConnectionManager& src);
		
		static std::list<Socket>		_socketList;
		static std::list<Connection *>	_connectionList;
		
		void	addListenSocket(Server& server);
		void	monitorConnections();
		static void	deleteConnection(Connection *connection);
	};