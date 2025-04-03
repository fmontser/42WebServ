#include <iostream>
#include <cstdlib>
#include "ServerConstants.hpp"
#include <sys/stat.h>
#include "Config.hpp"
#include <sstream>

std::string Config::_appRoot = "../";

static std::string toString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

void printError(const std::string &str) {
	std::cerr << RED << "Config file error: " << str << END << std::endl;
	exit(1);
}

bool printFalse(const std::string &str) {
	std::cerr << RED << str << END << std::endl;
	return false;
}

std::map<std::string, Server> Config::_servers;
Server* Config::_actualServer = NULL;
bool Config::_insideRouteBlock = false;

Config::Config() {}
Config::~Config() {}
Config::Config(const Config& src) {
	_appRoot = src._appRoot;
	_servers = src._servers;
}

Config& Config::operator=(const Config& src) {
	if (this != &src) {
		_appRoot = src._appRoot;
		_servers = src._servers;
	}
	return *this;
}

static void tokenizeServerMethods(std::stringstream& ss, std::vector<std::pair<std::string, std::vector<std::string> > > &tokenPairs) {
	std::string					buffer;
	std::vector<std::string>	values;

	while (true) {
		ss >> buffer;
		if (ss.fail())
			break;
		values.push_back(buffer);
	} 
	tokenPairs.push_back(std::make_pair(std::string("serverMethods"), values));
}

static void tokenize(std::fstream &configFileStream, std::vector<std::pair<std::string, std::vector<std::string> > > &tokenPairs) {
	std::string line;
	int lineNumber = 0;

	while (std::getline(configFileStream, line)) {
		lineNumber++;
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos) {
			line = line.substr(0, commentPos);
		}
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);
		if (line.empty()) {
			continue;
		}
		std::stringstream ss(line);
		std::string key;
		std::vector<std::string> values;
		ss >> key;
		if (key == "serverMethods") {
			tokenizeServerMethods(ss, tokenPairs);
			continue;
		}
		if (key.empty()) {
			continue;
		}
		if (key == "{" || key == "}") {
			tokenPairs.push_back(std::make_pair(key, std::vector<std::string>()));
			continue;
		}
		std::string value;
		while (ss >> value) {
			values.push_back(value);
		}
		if (values.empty() && key != "{" && key != "}") {
			printError("Line " + toString(lineNumber) + " has no values for key '" + key + "'.");
		}
		tokenPairs.push_back(std::make_pair(key, values));
	}
	configFileStream.close();
}

void Config::loadConfig(std::fstream &configFileStream) {
	std::vector<std::pair<std::string, std::vector<std::string> > > tokenPairs;
	tokenize(configFileStream, tokenPairs);

	for (std::vector<std::pair<std::string, std::vector<std::string> > >::iterator it = tokenPairs.begin(); it != tokenPairs.end(); ++it) {
		if (it->first == "server") {
			addServer(it);
		}
	}
}

void Config::addRoute(std::vector<std::pair<std::string, std::vector<std::string> > >::iterator &it) {
	Route						route;
	std::string					key;
	std::vector<std::string>    values;

	route.setUrl(it->second[0]);
	while ((++it)->first != "}") {
		key = it->first;
		values = it->second;

		if (key == "methods") {
			for (std::vector<std::string>::iterator mit = values.begin(); mit != values.end(); ++mit) {
				route.addMethod(std::make_pair("method", *mit));
			}
		} else if (key == "file" || key == "root" || key == "redirect" || key == "autoindex") {
			for (std::vector<std::string>::iterator vit = values.begin(); vit != values.end(); ++vit) {
				route.addFile(std::make_pair(key, *vit));
			}
		} else if (key == "cgi") { //TODO pendiente de implementacion CGI
			if (values.size() != 2)
				printError("Invalid cgi values in route " + route.getUrl());
			route.addFile(std::make_pair(key, values[0]));
		} else if (key == "default") {
			route.setDefault(values[0]);
		}
		else {
			printError("unknown token " + key);
		}
	}
	_actualServer->getRoutes().insert(std::make_pair(route.getUrl(), route));
}

void Config::addServer(std::vector<std::pair<std::string, std::vector<std::string> > >::iterator &it) {
	Server server;
	_actualServer = &server;
	if (it->second[0] == "{")
		printError("Server name is missing.");
	 server.setName(it->second[0]);
 
	bool foundOpeningBrace = false;
	for (std::vector<std::string>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
		if (*vit == "{") {
			foundOpeningBrace = true;
			break;
		}
	}
	if (!foundOpeningBrace) {
		printError("Config file error: Expected '{' after server name.");
	}
	while ((++it)->first != "}") {
		std::string key = it->first;
		std::vector<std::string> values = it->second;

		if (key == "maxPayload") {
			server.setMaxPayLoad(values[0]);
		} else if (key == "name") {
			server.setName(values[0]);
		} else if (key == "host") {
			server.setHost(values[0]);
		} else if (key == "port") {
			server.setPort(values[0]);
		} else if (key == "root") {
			server.setRoot(values[0]);
		} else if (key == "uploadDir") {
			server.setUploadDir(values[0]);
		} else if (key == "serverMethods") {
			server.setServerMethods(values);
		} else if (key.find("default",0) != key.npos) {
			server.getDefaults()[key] = values[0];
		} else if (key == "route") {
			_insideRouteBlock = true;
			addRoute(it);
			_insideRouteBlock = false;
		} else if (key == "autoindex") {
			if (!_insideRouteBlock) 
				printError("autoindex must be inside a route block.");
		}
	}
	if (!isValidConfig(server))
		printError("server " + server.getName() + " is invalid.");
	if (_servers.find(server.getName()) != _servers.end())
		printError("server " + server.getName() + " is duplicated.");
	else {
		_servers.insert(std::make_pair(server.getName(), server));
		_actualServer = &(_servers[server.getName()]);
	}
}

bool Config::isValidConfig(Server &server) {
	if (server.getName().empty())
		return printFalse("Server name is missing.");

	if (server.getMaxPayload() < MIN_PAYLOAD || server.getMaxPayload() > MAX_PAYLOAD)
		return printFalse("Server maxPayload is invalid.");

	if (server.getHost().empty())
		return printFalse("Server host is missing.");

	int port = server.getPort();
	if (port <= 0 || port > 65535)
		return printFalse("Server port is invalid.");

	if (server.getRoot().empty() || server.getRoot()[0] != '.')
		return printFalse("Server root is missing.");

	if (server.getUploadDir().empty())
		return printFalse("Server upload dir is missing.");

	for (std::vector<std::string>::iterator it = server.getServerMethods().begin(); it != server.getServerMethods().end(); it++) {
			std::string method = it->c_str();
			if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
				return printFalse("Invalid server method '" + method + "' in server " + server.getName());
	}



	std::map<std::string, Route>& _routes = server.getRoutes();
	for (std::map<std::string, Route>::const_iterator it = _routes.begin(); it != _routes.end(); ++it) {
		if (it->first.empty() || it->first[0] == '{')
			return printFalse("Route name is missing.");
		const Route& route = it->second;

		if (route.getUrl().empty())
			return printFalse("Route: " + it->first + "url is missing");
		if (route.getDefault().empty())
			return printFalse("Route: " + it->first + "default file is missing");

		const std::multimap<std::string, std::string>& methods = route.getMethods();
		for (std::multimap<std::string, std::string>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit) {
			if (mit->second != "GET" && mit->second != "POST" && mit->second != "PUT" && mit->second != "DELETE")
				return printFalse("Invalid method '" + mit->second + "' in route " + route.getUrl());
		}

		//TODO need?
		const std::multimap<std::string, std::string>& files = route.getFiles();
		for (std::multimap<std::string, std::string>::const_iterator fit = files.begin(); fit != files.end(); ++fit) {
			if (fit->first == "root" && ((fit->second[0] != '.' && (fit->second[1] != '/' || fit->second[1] != '\0') )))
				return printFalse("Invalid root path '" + fit->second + "' in route " + route.getUrl());
			if (fit->first == "redirect" && (fit->second[0] != '.' || fit->second[1] != '/'))
				return printFalse("Invalid redirect path '" + fit->second + "' in route " + route.getUrl());
			if ((fit->first == "autoindex" && (fit->second != "on" && fit->second != "off")))
				return printFalse("Invalid autoindex value " + fit->second );
		}
	}
	return true;
}

std::map<std::string, Server>& Config::getServers() { return _servers; }
std::string Config::getAppRoot() { return _appRoot; }