#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.hpp"
#include "ServerConstants.hpp"
#include "PathManager.hpp"
#include "Utils.hpp"

Server::Server() {
	_defaults["default201"] = "defaults/201.html";
	_defaults["default204"] = "defaults/204.html";
	_defaults["default400"] = "defaults/400.html";
	_defaults["default403"] = "defaults/403.html";
	_defaults["default404"] = "defaults/404.html";
	_defaults["default405"] = "defaults/405.html";
	_defaults["default409"] = "defaults/409.html";
	_defaults["default413"] = "defaults/413.html";
	_defaults["default415"] = "defaults/415.html";
	_defaults["default500"] = "defaults/500.html";
	_defaults["default501"] = "defaults/501.html";
	_defaults["default504"] = "defaults/504.html";
}

Server::~Server() {}

Server::Server(const Server& src) {	
	_name = src._name;
	_hosts = src._hosts;
	_port = src._port;
	_root = src._root;
	_defaults = src._defaults;
	_maxPayload = src._maxPayload;
	_routes = src._routes;
	_serverMethods = src._serverMethods;
}

Server& Server::operator=(const Server& src) {
	if (this != &src) {
		_name = src._name;
		_hosts = src._hosts;
		_port = src._port;
		_root = src._root;
		_defaults = src._defaults;
		_maxPayload = src._maxPayload;
		_routes = src._routes;
		_serverMethods = src._serverMethods;
	}
	return *this;
}

std::string							Server::getName() const { return this->_name; }
std::vector<std::string>&			Server::getHosts() { return this->_hosts; }
int									Server::getPort() const { return this->_port; }
std::string							Server::getRoot() const { return this->_root; }
std::vector<std::string>&			Server::getServerMethods() { return this->_serverMethods; }
std::map<std::string, std::string>&	Server::getDefaults() {return this->_defaults; }
size_t								Server::getMaxPayload() const { return _maxPayload; }
std::map<std::string, Route>&		Server::getRoutes() { return _routes; }

void	Server::setName(const std::string& name) { this->_name = name; }
void	Server::setHosts(const std::vector<std::string>& hosts) { this->_hosts = hosts; }
void	Server::setRoot(const std::string& root) {_root = root; }
void	Server::setServerMethods(const std::vector<std::string>& serverMethods) { _serverMethods = serverMethods; }


void	Server::setPort(const std::string& port) {
	char	*err;
	int		portNumber;
	
	portNumber = strtol(port.c_str(), &err, 10);
	_port = portNumber;
}

void	Server::setMaxPayLoad(const std::string& maxPayLoad) {
	char	*err;
	int		payloadSize;
	
	payloadSize = strtol(maxPayLoad.c_str(), &err, 10);
	_maxPayload = payloadSize;
}


HttpResponse::responseType	Server::getRequestedRoute(Route **route, DataAdapter& dataAdapter) {
	std::string path, url;

	url = dataAdapter.getRequest().url;
	if (dataAdapter.getRequest().isCgiRequest) {
		url = CgiAdapter::stripCgiParams(url);
		url = CgiAdapter::stripCgiPathInfo(url);
	}

	path = PathManager::resolveServerPath(dataAdapter);

	if (!Utils::isDirectory(path))
		url = Utils::getPathDir(url);

	if (!(access(path.c_str(), F_OK) == 0))
		return HttpResponse::NOT_FOUND;
	if (!(access(path.c_str(), R_OK) == 0))
		return HttpResponse::FORBIDDEN;

	std::map<std::string, Route>::iterator it = _routes.find(url);
	if (it !=  _routes.end()) {
		*route = &it->second;
		return HttpResponse::EMPTY;
	}

	it = _routes.find(url.append("/"));
	if (it !=  _routes.end()) {
		*route = &it->second;
		return HttpResponse::EMPTY;
	}
	return HttpResponse::FORBIDDEN;
}