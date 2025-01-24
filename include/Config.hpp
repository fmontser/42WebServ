#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Route.hpp"
#include "Server.hpp"

class Config {
	private:
		int								_maxPayload;
		std::map<std::string, Route>	_routes;
		std::map<std::string, Server>	_servers;

		void	setMaxPayload(std::vector<std::string>::iterator &it);
		void	addRoute(std::vector<std::string>::iterator &it);
		void	addServer(std::vector<std::string>::iterator &it);

	public:
		Config();
		~Config();
		Config(const Config& src);
		Config& operator=(const Config& src);

		void	loadConfig(std::fstream &configFileStream);

		int								getMaxPayload() const;
		std::map<std::string, Route>&	getRoutes() const;
		std::map<std::string, Server>&	getServers() const;
};

