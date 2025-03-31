#include <iostream>
#include <cstdlib>
#include "ServerConstants.hpp"
#include <sys/stat.h>
#include "Config.hpp"

void printError(const std::string &str) {
	std::cerr << RED << str << END << std::endl;
	exit(1);
}

bool printFalse(const std::string &str) {
	std::cerr << RED << str << END << std::endl;
	return false;
}

std::map<std::string, Server> Config::_servers;
Server* Config::_actualServer = NULL; 
/* bool isRoute = false; */

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

bool Config::isValidConfig(Server &server) {
	if (server.getName().empty())
		return printFalse("Config file error: Server name is missing.");

	if (server.getMaxPayload() < MIN_PAYLOAD || server.getMaxPayload() > MAX_PAYLOAD)
		return printFalse("Config file error: Server maxPayload is invalid.");

	if (server.getHost().empty())
		return printFalse("Config file error: Server host is missing.");

	int port = server.getPort();
	if (port <= 0 || port > 65535)
		return printFalse("Config file error: Server port is invalid.");

	if (server.getRoot().empty() || server.getRoot()[0] != '.')
		return printFalse("Config file error: Server root is missing.");

	std::string defaultPage = server.getDefault();
	if (!defaultPage.empty() && (defaultPage.size() < 5 || defaultPage.substr(defaultPage.size() - 5) != ".html"))
		return printFalse("Config file error: Server default must end with .html");

	const std::map<std::string, Route>& _routes = server.getRoutes();
	for (std::map<std::string, Route>::const_iterator it = _routes.begin(); it != _routes.end(); ++it) {
		
		if (it->first.empty() || it->first[0] == '{')
			return printFalse("Config file error: Route url is missing.");
		const Route& route = it->second;

		const std::multimap<std::string, std::string>& methods = route.getMethods();
		for (std::multimap<std::string, std::string>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit) {
			if (mit->second != "GET" && mit->second != "POST" && mit->second != "PUT" && mit->second != "DELETE")
				return printFalse("Config file error: Invalid method '" + mit->second + "' in route " + route.getUrl());
		}

		const std::multimap<std::string, std::string>& files = route.getFiles();
		for (std::multimap<std::string, std::string>::const_iterator fit = files.begin(); fit != files.end(); ++fit) {
			if (fit->first == "root" && (fit->second[0] != '.' || fit->second[1] != '/'))
				return printFalse("Config file error: Invalid root path '" + fit->second + "' in route " + route.getUrl());
			if (fit->first == "redirect" && (fit->second[0] != '.' || fit->second[1] != '/'))
				return printFalse("Config file error: Invalid redirect path '" + fit->second + "' in route " + route.getUrl());
			if ((fit->first == "autoindex" && (fit->second != "on" && fit->second != "off"))/*  || !isRoute */)
				return printFalse("Config file error: Invalid autoindex value " + fit->second );
		}
	}
	return true;
}

static void tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList) {
	char c;
	std::string token;
	bool flag = false;
	bool isComment = false;

	token.clear();
	while (configFileStream.get(c)) {
		if (c == '#')
			isComment = true;
		if (isComment && c == '\n')
			isComment = false;
		else if (isComment)
			continue;
		if (isspace(c)) {
			if (flag) {
				flag = false;
				tokenList.push_back(token);
				token.clear();
			}
			else
				continue;
		}
		else {
			flag = true;
			token.append(std::string(1, c));
		}
	}
	tokenList.push_back(token);
	configFileStream.close();
}

void Config::loadConfig(std::fstream &configFileStream) {
	std::vector<std::string> tokenList;
	std::vector<std::string>::iterator it;

	tokenize(configFileStream, tokenList);


	for (it = tokenList.begin(); it != tokenList.end(); ++it) {
		if (*it == "server")
			addServer(it);
	}
}

void Config::addRoute(std::vector<std::string>::iterator &it) {
	Route route;
	std::string key, value;
 
	route.setUrl(*(++it));

	if (*(++it) == "{") {
		while (*(++it) != "}") {
			key = *it;
			value = *(++it);
			if (key == "methods") {
				while (42) {
					if (*it == "GET" || *it == "POST" || *it == "DELETE") {
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
			else if (key == "root") {
				route.addFile(std::make_pair(key, value));
			}
			else if (key == "redirect") {
				route.addFile(std::make_pair(key, value));
			}
			else if (key == "autoindex") {
				route.addFile(std::make_pair(key, value));
			}
			else {
				printError("Config file error: unknown token " + key);
			}
		   /*  isRoute = false; */
		}
	}

 /*    if (_actualServer->getRoutes().find(route.getUrl()) != _actualServer->getRoutes().end())
		printError("Config file error: Route " + route.getUrl() + " is duplicated.");
	else
  */       _actualServer->getRoutes().insert(std::make_pair(route.getUrl(), route));
}

void Config::addServer(std::vector<std::string>::iterator &it) {
	Server server;
	_actualServer = &server;

	server.setName(*(++it));
	if (*(++it) == "{") {
		while (*(++it) != "}") {
			if (*it == "maxPayload") {
				server.setMaxPayLoad(*(++it));
			}
			else if (*it == "name") {
				server.setName(*(++it));
			}
			else if (*it == "host") {
				server.setHost(*(++it));
			}
			else if (*it == "port") {
				server.setPort(*(++it));
			}
			else if (*it == "root") {
				server.setRoot(*(++it));
			}
			else if (*it == "default") {
				server.setDefault(*(++it));
			}
			else if (*it == "route") {
			  /*   isRoute = true; */
				addRoute(it);
			}
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
/*             else {
				printError("Config file error: unknown token " + *it);
			} */
		}
		if (!isValidConfig(server))
			printError("Config file error: server " + server.getName() + " is invalid.");
	}

	if (_servers.find(server.getName()) != _servers.end())
		printError("Config file error: server " + server.getName() + " is duplicated.");
	else {
		_servers.insert(std::make_pair(server.getName(), server));
		_actualServer = &(_servers[server.getName()]);
	}
}

std::map<std::string, Server>& Config::getServers() {
	return _servers;
}