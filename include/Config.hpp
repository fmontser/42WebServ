#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Route.hpp"
#include "Server.hpp"

class Config {
	private:
		static int								_maxPayload;
		static std::map<std::string, Route>		_routes;
		static std::map<std::string, Server>	_servers;

		static void	setMaxPayload(std::vector<std::string>::iterator &it);
		static void	addRoute(std::vector<std::string>::iterator &it);
		static void	addServer(std::vector<std::string>::iterator &it);

		Config();
		~Config();
		Config(const Config& src);
		Config& operator=(const Config& src);
	public:
		static void	loadConfig(std::fstream &configFileStream);

		static int								getMaxPayload();
		static std::map<std::string, Route>&	getRoutes();
		static std::map<std::string, Server>&	getServers();
};

