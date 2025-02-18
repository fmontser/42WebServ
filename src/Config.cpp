#include <iostream>
#include <cstdlib>
#include "ServerConstants.hpp"
#include <sys/stat.h>
#include "Config.hpp"

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
	if (server.getName().empty()) {
		std::cerr << RED << "Config file error: Server name is missing." << END << std::endl;
		return false;
	}
	if (server.getHost().empty()) {
		std::cerr << RED << "Config file error: Server host is missing." << END << std::endl;
		return false;
	}
	if (port == 0 || (port < 0 || port > 65535)) {
		std::cerr << RED << "Config file error: Server port is missing." << END << std::endl;
		return false;
	}
	if (server.getMaxPayload() == 0) {
		std::cerr << RED << "Config file error: Server maxPayload is missing." << END << std::endl;
		return false;
	}
	if (server.getRoot().empty()) {
		std::cerr << RED << "Config file error: Server root is missing." << END << std::endl;
		return false;
	}
/* 	if (server.getRoutes().empty())  {
		std::cerr << RED << "Config file error: Server root is missing." << END << std::endl;
		return false;
	} */
/* 	if (routes.empty() || routes.find(server.getDefault()) == routes.end()) {
		std::cerr << RED << "Config file error: Server default route is missing." << END << std::endl;
		return false;
	} */
/* 	for (std::map<std::string, Route>::iterator it = routes.begin(); it != routes.end(); ++it) {
		std::string routePath = it->first;
		std::string filePath = server.getRoot() + routePath;

		struct stat buffer;
		if (stat(filePath.c_str(), &buffer) != 0) {
			std::cerr << RED << "Config file error: Route " << routePath << " file path is invalid." << END << std::endl;
			return false;
		}
		if (it->second.getMethods().empty()) {
			std::cerr << RED << "Config file error: Route " << it->first << " has no methods." << END << std::endl;
			return false;
		}
		if (it->second.getFiles().empty()) {
			std::cerr << RED << "Config file error: Route " << it->first << " has no files." << END << std::endl;
			return false;
		}
	} */
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
					++it;
					if (*it == "\n") {
						--it;
						break;
					}
					route.addMethod(std::make_pair("method", *it));
				}
				//route.addMethod(std::make_pair(key, value));
			}
			else if (key == "file")
				route.addFile(std::make_pair(key, value));
			else if (key == "autoindex") 
				std::cout << "autoindex: " << value << std::endl;
				//TODO
			else if (key == "root") 
				std::cout << "root: " << value << std::endl;
				//TODO
			else {
				std::cerr << RED << "Config file error: unknown token " << key << END << std::endl;
				exit(1);
			}
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
		//	exit(1);
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

/* 
std::string	Config::get400Page() {
	try
	{
		if (_actualServer == NULL) {
			throw std::runtime_error("Bad request");
		}
		std::size_t pos = _actualServer->getRoot().find("400");
		if (pos != std::string::npos) {
			return _actualServer->getRoot().substr(pos);
		} else {
			throw std::runtime_error("400 Bad Request");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return "";
}

std::string	Config::get404Page() {
	try
	{
		if (_actualServer == NULL) {
			throw std::runtime_error("No actual server set");
		}
		std::size_t pos = _actualServer->getRoot().find("404");
		if (pos != std::string::npos) {
			return _actualServer->getRoot().substr(pos);
		} else {
			throw std::runtime_error("404 page not found");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return "";
}

std::string	Config::get500Page() {
	try {
		if (_actualServer == NULL) {
			throw std::runtime_error("No actual server set");
		}
		std::size_t pos = _actualServer->getRoot().find("500");
		if (pos != std::string::npos) {
			return _actualServer->getRoot().substr(pos);
		} else {
			throw std::runtime_error("500 Internal Server Error");
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return "";
}

std::string	Config::get501Page() {
	try	{
		if (_actualServer == NULL) {
			throw std::runtime_error("No actual server set");
		}
		std::size_t pos = _actualServer->getRoot().find("501");
		if (pos != std::string::npos) {
			return _actualServer->getRoot().substr(pos);
		} else {
			throw std::runtime_error(YELLOW "501 Not Implemented" END);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return "";
}

std::string	Config::getErrorPage(int errorCode) {
	if (errorCode == 400)
		return get400Page();
	else if (errorCode == 403)
		return get403Page();
	else if (errorCode == 404)
		return get404Page();
	else if (errorCode == 500)
		return get500Page();
	else if (errorCode == 501)
		return get501Page();
	return "";
} */