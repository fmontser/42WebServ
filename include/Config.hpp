#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Server.hpp"

class Config {
private:
	static std::map<std::string, Server>	_servers;
	static Server*							_actualServer;
	static bool								_insideRouteBlock;

	static void addRoute(std::vector<std::pair<std::string, std::vector<std::string> > >::iterator &it);
	static void addServer(std::vector<std::pair<std::string, std::vector<std::string> > >::iterator &it);
	static bool isValidConfig(Server &server);

	Config();
	~Config();
	Config(const Config& src);
	Config& operator=(const Config& src);

public:
	static void loadConfig(std::fstream &configFileStream);
	static std::map<std::string, Server>& getServers();
};