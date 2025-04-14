#pragma once

#include <map>
#include <vector>
#include <string>
#include <list>
#include <poll.h>
#include "Route.hpp"

class Connection;
class DataAdapter;

class Server {
	private:
		std::string							_name;
		std::string							_host;
		int									_port;
		size_t								_maxPayload;
		std::string							_root;
		std::vector<std::string>			_serverMethods;
		std::map<std::string, std::string>	_defaults;
		std::map<std::string, Route>		_routes;
		std::list<Connection *>				_connectionList;
		int									_socketFd;
		struct pollfd						_pollfd;

	public:
		Server();
		~Server();
		Server(const Server& src);
		Server& operator=(const Server& src);

		std::string							getName() const;
		std::string							getHost() const;
		int									getPort() const;
		std::string							getRoot() const;
		std::vector<std::string>&			getServerMethods();
		std::map<std::string, std::string>&	getDefaults();
		size_t								getMaxPayload() const;
		std::map<std::string, Route>&		getRoutes();
		std::list<Connection *>&			getConnectionList();
		int									getSocketFd() const;
		struct pollfd						getPollfd() const;

		void	setName(const std::string& name);
		void	setHost(const std::string& host);
		void	setPort(const std::string& port);
		void	setRoot(const std::string& root);
		void	setServerMethods(const std::vector<std::string>& serverMethods);
		void	setSocketFd(int socketFd);
		void	setPollfd(struct pollfd pfd);
		void	setMaxPayLoad(const std::string& maxPayLoad);

		Route	*getRequestedRoute(DataAdapter& dataAdapter);
		void	listenSocket();
		bool	hasPollIn() const;
		bool	hasPollOut() const;
};
