#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Route.hpp"
#include "Server.hpp"

class Config {
	private:
		//TODO borrar std::map<std::string, void (Config::*)(std::vector<std::string>::iterator &it)>	_tokenMap;

		int								_maxPayload;
		std::map<std::string, Route>	_routes;
		std::map<std::string, Server>	_servers;

		Config(const Config& src);
		Config& operator=(const Config& src);

		void	setMaxPayload(std::vector<std::string>::iterator &it);
		void	setRoutes(std::vector<std::string>::iterator &it);
		void	setServers(std::vector<std::string>::iterator &it);

		void	loadConfig(std::fstream &configFileStream);
	public:
		Config();
		~Config();

		int								getMaxPayload() const;
		std::map<std::string, Route>&	getRoutes() const;
		std::map<std::string, Server>&	getServers() const;
};

