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

		Config(const Config& src);
		Config& operator=(const Config& src);

		void	setMaxPayload(std::vector<std::string>::iterator &it);
		void	setRoutes(std::vector<std::string>::iterator &it);
		void	setServers(std::vector<std::string>::iterator &it);

		void	tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList);

	public:
		Config();
		~Config();

		void	loadConfig(std::fstream &configFileStream);

		int								getMaxPayload() const;
		std::map<std::string, Route>&	getRoutes() const;
		std::map<std::string, Server>&	getServers() const;
};

