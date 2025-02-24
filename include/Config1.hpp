/* #pragma once

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


		static std::map<std::string, Server>&	getServers();
};
 */

#pragma once

#include <map>
#include <vector>
#include <fstream>
#include "Server.hpp"

class Config {
    private:
        static std::map<std::string, Server> _servers;
        static Server* _actualServer;

        static bool isValidConfig(Server &server);
        static void tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList);
        static void addServer(std::vector<std::string>::iterator &it);
        static void addRoute(std::vector<std::string>::iterator &it);

    public:
        Config();
        ~Config();
        Config(const Config& src);
        Config& operator=(const Config& src);

        void loadConfig(std::fstream &configFileStream);
        static std::map<std::string, Server>& getServers();
};