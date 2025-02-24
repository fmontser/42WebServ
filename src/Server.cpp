#include <cstdlib>
#include <iostream>
#include "Server.hpp"
#include "ServerConstants.hpp"

Server::Server() {}
Server::~Server() {}
Server::Server(const Server& src) {	
	_name = src._name;
	_host = src._host;
	_port = src._port;
	_root = src._root;
	_default = src._default;
	_maxPayload = src._maxPayload;
	_routes = src._routes;
	_socketList = src._socketList;
	_method = src._method;
}

Server& Server::operator=(const Server& src) {
	if (this != &src) {
		_name = src._name;
		_host = src._host;
		_port = src._port;
		_root = src._root;
		_default = src._default;
		_maxPayload = src._maxPayload;
		_routes = src._routes;
		_socketList = src._socketList;
		_method = src._method;
	}
	return *this;
}

std::string						Server::getName() const { return this->_name; }
std::string						Server::getHost() const { return this->_host; }
int										Server::getPort() const { return this->_port; }
std::string						Server::getRoot() const { return this->_root; }
int										Server::getMaxPayload() const { return _maxPayload; }

std::map<std::string, Route>&	Server::getRoutes() const { 
	return (std::map<std::string, Route>&) _routes; }
std::list<Socket *>&			Server::getSocketList() { return _socketList; }
std::string						Server::getDefault() const { return _default; }

void	Server::setName(const std::string& name) { this->_name = name; }
void	Server::setHost(const std::string& host) { this->_host = host; }
void	Server::setRoot(const std::string& root) {_root = root; }
void	Server::setDefault(const std::string& default_) {_default = default_; }
void	Server::addConfigMethods(const std::string& method) {_method[method] = "allowed"; }
//TODO####################
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
