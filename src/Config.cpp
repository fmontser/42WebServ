#include <iostream>
#include <cstdlib>
#include "ServerConstants.hpp"
#include <sys/stat.h>
#include "Config.hpp"

void	printError(std::string str) {
	std::cout << RED << str <<  END << std::endl;
	exit(1);
}

bool printFalse(std::string str) {
	std::cout << RED << str <<  END << std::endl;
	return false;
}

std::map<std::string, Server>	Config::_servers;
Server							*Config::_actualServer = NULL;

Config::Config() {}
Config::~Config() {}
Config::Config(const Config& src) {
	_servers = src._servers;
}

Config& Config::operator=(const Config& src) {
	if (this != &src) {
		_servers = src._servers;
	}
	return *this;
}

static std::map<std::string, void (*)(std::vector<std::string>::iterator &it)>	_tokenMap;

static bool isValidConfig(Server server){
	int port = server.getPort();
	std::map<std::string, Route> routes = server.getRoutes();
	if (server.getName().empty()) 
		return printFalse("Config file error: Server name is missing.");
/* 	{
		std::cerr << RED << "Config file error: Server name is missing." << END << std::endl;
		return false;
	}
 */	
	if (server.getHost().empty()) {
		std::cerr << RED << "Config file error: Server host is missing." << END << std::endl;
		return false;
	}
	if (isalpha(port) || port == 0 || (port < 0 || port > 65535)) {
		std::cerr << RED << "Config file error: Server port is missing." << END << std::endl;
		return false;
	}
	if (server.getMaxPayload() == 0) {
		std::cerr << RED << "Config file error: Server maxPayload is missing." << END << std::endl;
		return false;
	}
	//TODO	TODO y	TODO
	if (server.getMaxPayload() < MIN_PAYLOAD || server.getMaxPayload() > MAX_PAYLOAD || isalpha(server.getMaxPayload())) {
		std::cerr << RED << "Config file error: Server maxPayload is invalid." << END << std::endl;
		return false;
	}
	if (server.getRoot().empty()) {
		std::cerr << RED << "Config file error: Server root is missing." << END << std::endl;
		return false;
	}
/* 			else if (key == "maxPayload") {
				char *end;
				long maxPValue = strtol(value.c_str(), &end, 10);
				if (maxPValue < MIN_PAYLOAD || maxPValue > MAX_PAYLOAD || !isalnum(maxPValue)) 
					printError("Config file error: invalid maxPayload value " + value);
			}
 */

	return true;
}

static void	tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList){
	char		c;
	std::string	token;
	bool		flag = false;
	bool		isComment = false;

	token.clear();
	while (configFileStream.get(c)) {
		if (c == '#') 
			isComment = true;
		if (isComment && c == '\n')
			isComment = false;
		else if (isComment)
			continue;
		if (isspace(c)) {
			if (flag){
				flag = false;
				tokenList.push_back(token);
				token.clear();
			}
			else
				continue;
		}
		else {
			flag = true;
			token.append(std::string(1,c));
		}
	}
	tokenList.push_back(token);
	configFileStream.close();
}

void	Config::loadConfig(std::fstream &configFileStream) {
	
	std::vector<std::string>			tokenList;
	std::vector<std::string>::iterator	it;
	
	tokenize(configFileStream, tokenList);


	for (it = tokenList.begin(); it != tokenList.end(); ++it){
		if (*it == "server") 
			addServer(it);
	}
}

//TODO cambiar lo del tokenmap!!
//TODO if else meh... (hacer metodos estaticos y anadirlos al tokenmap??)
void	Config::addRoute(std::vector<std::string>::iterator &it) {
	Route route;
	std::string key, value;

	route.setUrl(*(++it));//TODO TODO TODO
	if (*(++it) == "{") {
		while(*(++it) != "}") {
			key = *it;
			value = *(++it);
			if (key == "methods") {
				while (42) {
					if (*it == "GET" || *it == "POST" || *it == "PUT" || *it == "DELETE") {
						route.addMethod(std::make_pair("method", *it));
					}
					else {
						--it;
						break;
					}
					++it;
				}
			}
			else if (key == "file") {
				
				route.addFile(std::make_pair(key, value));
			}
			else if (key == "autoindex") {
				if (value != "on" && value != "off") 
					printError("Config file error: invalid autoindex value " + value);
				route.setAutoIndex(std::make_pair(key, value));
			}
/* 			else if (key == "maxPayload") {
				char *end;
				long maxPValue = strtol(value.c_str(), &end, 10);
				if (maxPValue < MIN_PAYLOAD || maxPValue > MAX_PAYLOAD || !isalnum(maxPValue)) 
					printError("Config file error: invalid maxPayload value " + value);
			}
 */
			else if (key == "root") {
				if (value[0] != '.' && value[1] != '/') 
					printError("Config file error: invalid root path " + value);

				route.setRoot(std::make_pair(key, value));
			}
				//TODO
			else if (key == "redirect") {
				if (value[0] != '.' && value[1] != '/') 
					printError("Config file error: invalid redirect path " + value);

				route.setRedirect(std::make_pair(key, value));
		}
				//TODO
			else
				printError("Config file error: unknown token " + key);

		}
	}
	if (_actualServer->getRoutes().find(route.getUrl()) != _actualServer->getRoutes().end()) {
		std::cerr << "Config file error: Route " << route.getUrl()<< " is duplicated." << std::endl;
		exit(1); //TODO terminate
	}
	else
		_actualServer->getRoutes().insert(std::make_pair(route.getUrl(), route));
}

//TODO if else meh...(hacer metodos estaticos y anadirlos al tokenmap??)
void	Config::addServer(std::vector<std::string>::iterator &it) {
	Server server;
	_actualServer = &server;
	
	server.setName(*(++it));
	if (*(++it) == "{") {
		while(*(++it) != "}") {
				if (*it == "maxPayload")
					server.setMaxPayLoad(*(++it));
				else if (*it == "name")
					server.setName(*(++it));
				else if (*it == "host")
					server.setHost(*(++it));
				else if (*it == "port")
					server.setPort(*(++it));
				else if (*it == "root")
					server.setRoot(*(++it));
				else if (*it == "default")
					server.setDefault(*(++it));
				else if (*it == "route")
					addRoute(it);
				else if (*it == "methods") {
					while (42) {
						++it;
						if (*it == "}") {
							--it;
							break;
						}
						server.addConfigMethods(*it);
					} 
				}
				else {
					std::cerr << RED << "Config file error: unknown token " << *it << END << std::endl;
					exit(1);
				}
				
		}
		if (!isValidConfig(server)) {
			std::cerr << RED << "Config file error: server " << server.getName()<< " is invalid." << END << std::endl;
			exit(1);
		}
	}

	if (_servers.find(server.getName()) != _servers.end()) {
		std::cerr << "Config file error: server " << server.getName()<< " is duplicated." << std::endl;
		exit(1); 
	}
	else {
		_servers.insert(std::make_pair(server.getName(), server));
		_actualServer = &(_servers[server.getName()]);
	}
}

std::map<std::string, Server>&	Config::getServers() { return (std::map<std::string, Server>&)_servers; }