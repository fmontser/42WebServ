#pragma once

#include <map>
#include <vector>
#include <string>
#include <list>
#include <poll.h>
#include "Route.hpp"
#include "HttpResponse.hpp"

class DataAdapter;

class Server {
	private:
	
		std::string							_name;
		std::vector<std::string>			_hosts;
		int									_port;
		size_t								_maxPayload;
		std::string							_root;
		std::vector<std::string>			_serverMethods;
		std::map<std::string, std::string>	_defaults;
		std::map<std::string, Route>		_routes;


	public:

		Server();
		~Server();
		Server(const Server& src);
		Server& operator=(const Server& src);

		std::string							getName() const;
		std::vector<std::string>&			getHosts();
		int									getPort() const;
		std::string							getRoot() const;
		std::vector<std::string>&			getServerMethods();
		std::map<std::string, std::string>&	getDefaults();
		size_t								getMaxPayload() const;
		std::map<std::string, Route>&		getRoutes();

		void	setName(const std::string& name);
		void	setHosts(const std::vector<std::string>& hosts);
		void	setPort(const std::string& port);
		void	setRoot(const std::string& root);
		void	setServerMethods(const std::vector<std::string>& serverMethods);
		void	setMaxPayLoad(const std::string& maxPayLoad);

		HttpResponse::responseType	getRequestedRoute(Route **route, DataAdapter& dataAdapter);
}

