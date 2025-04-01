#pragma once

#include <map>
#include <vector>
#include <string>
#include <list>
#include <poll.h>
#include "Route.hpp"

class Connection;

class Server {
	private:
		std::string							_name;
		std::string							_host;
		int									_port;
		int									_maxPayload;
		std::string							_root;
		std::string							_uploadDir;
		std::vector<std::string>			_serverMethods;
		std::string							_default;
		std::map<std::string, Route>		_routes;
		std::list<Connection *>				_connectionList;
		int									_socketFd;
		struct pollfd						_pollfd;

	public:
		Server();
		~Server();
		Server(const Server& src);
		Server& operator=(const Server& src);


		std::string						getName() const;
		std::string						getHost() const;
		int								getPort() const;
		std::string						getRoot() const;
		std::string						getUploadDir() const;
		std::vector<std::string>&		getServerMethods();
		int								getMaxPayload() const;
		std::map<std::string, Route>&	getRoutes();
		std::list<Connection *>&		getConnectionList();
		int								getSocketFd() const;
		struct pollfd					getPollfd() const;


		void	setName(const std::string& name);
		void	setHost(const std::string& host);
		void	setPort(const std::string& port);
		void	setRoot(const std::string& root);
		void	setUploadDir(const std::string& uploadDir);
		void	setServerMethods(const std::vector<std::string>& serverMethods);
		void	setSocketFd(int socketFd);
		void	setPollfd(struct pollfd pfd);
		void	setMaxPayLoad(const std::string& maxPayLoad);

		void	listenSocket();
		bool	hasPollIn() const;
		bool	hasPollOut() const;
};
