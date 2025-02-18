#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Server.hpp"

class Config {
	private:
		static std::map<std::string, Server>	_servers;
		static Server							*_actualServer;

		static void	addRoute(std::vector<std::string>::iterator &it);
		static void	addServer(std::vector<std::string>::iterator &it);
		
		Config();
		~Config();
		Config(const Config& src);
		Config& operator=(const Config& src);
	public:
		static void	loadConfig(std::fstream &configFileStream);
		/* static std::string	get400Page();
		static std::string	get403Page();
		static std::string	get404Page();
		static std::string	get500Page();
		static std::string	get501Page();
		static std::string	getErrorPage(int errorCode); */

		static std::map<std::string, Server>&	getServers();
};

