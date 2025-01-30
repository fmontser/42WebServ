#pragma once

#include <map>
#include <string>
#include <list>
#include "Route.hpp"
#include "Socket.hpp"

class Server {
	private:
		std::string						_name;
		std::string						_host;
		int								_port;
		int								_maxPayload;
		std::map<std::string, Route>	_routes;
		std::list<Socket>				_socketList;

	public:
		Server();
		~Server();
		Server(const Server& src);
		Server& operator=(const Server& src);

	std::string						getName() const;
	std::string						getHost() const;
	int								getPort() const;
	int								getMaxPayload();
	std::map<std::string, Route>&	getRoutes();
	std::list<Socket>&				getSocketList();


	void	setName(const std::string& name);
	void	setHost(const std::string& host);
	void	setPort(const std::string& port);
	void	setMaxPayLoad(const std::string& maxPayLoad);
};
