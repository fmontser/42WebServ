#include <iostream>
#include <cstdlib>
#include "ServerConstants.hpp"
#include "Config.hpp"

std::map<std::string, Server> Config::_servers;

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

static void	tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList){
	char		c;
	std::string	token;
	bool		flag = false;
	
	token.clear();
	while (configFileStream.get(c)) {
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

	_tokenMap["maxPayload"] = &Config::setMaxPayload;
	_tokenMap["route"] = &Config::addRoute;
	_tokenMap["server"] = &Config::addServer;
	_tokenMap["method"] = NULL;
	_tokenMap["file"] = NULL;
	_tokenMap["port"] = NULL;
	_tokenMap["host"] = NULL;


	for (it = tokenList.begin(); it != tokenList.end(); ++it){
		if (_tokenMap.find(*it) != _tokenMap.end() && _tokenMap[*it]) 
			(_tokenMap[*it])(it);
	}
}

void	Config::setMaxPayload(std::vector<std::string>::iterator &it) {
	char	*err;
	int		payloadSize;
	
	++it;
	payloadSize = strtol((*it).c_str(), &err, 10);
	if (payloadSize < MIN_PAYLOAD || payloadSize > MAX_PAYLOAD || isalpha(*err)) {
		std::cerr << "Config file error: Ivalid payload size." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Payload size set as: " << payloadSize << " bytes." << std::endl;
	//_maxPayload = payloadSize;
}

//TODO if else meh... (hacer metodos estaticos y anadirlos al tokenmap??)
void	Config::addRoute(std::vector<std::string>::iterator &it) {
	Route route;
	std::string key, value;

	route.setUrl(*(++it));
	if (*(++it) == "{") {
		while(*(++it) != "}") {
			if (_tokenMap.find(*it) != _tokenMap.end()){
				key = *it;
				++it;
			}
			value = *it;
			if (key == "method")
				route.addMethod(std::make_pair(key, value));
			else if (key == "file")
				route.addFile(std::make_pair(key, value));
		}
	}

	if (_routes.find(route.getUrl()) != _routes.end()) {
		std::cerr << "Config file error: Route " << route.getUrl()<< " is duplicated." << std::endl;
		exit(1);
	}
	else
		_routes.insert(std::make_pair(route.getUrl(), route));
}

//TODO if else meh...(hacer metodos estaticos y anadirlos al tokenmap??)
void	Config::addServer(std::vector<std::string>::iterator &it) {
	Server server;

	server.setName(*(++it));
	if (*(++it) == "{") {
		while(*(++it) != "}") {
			if (_tokenMap.find(*it) != _tokenMap.end()){
				if (*it == "name")
					server.setName(*(++it));
				else if (*it == "host")
					server.setHost(*(++it));
				else if (*it == "port")
					server.setPort(*(++it));
			}
		}
	}
	if (_servers.find(server.getName()) != _servers.end()) {
		std::cerr << "Config file error: server " << server.getName()<< " is duplicated." << std::endl;
		exit(1);
	}
	else
		_servers.insert(std::make_pair(server.getName(), server));
}

int								Config::getMaxPayload() { return _maxPayload; }
std::map<std::string, Route>&	Config::getRoutes() { return (std::map<std::string, Route>&)_routes; }
std::map<std::string, Server>&	Config::getServers() { return (std::map<std::string, Server>&)_servers; }