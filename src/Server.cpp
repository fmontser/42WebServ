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
	_maxPayload = src._maxPayload;
	_routes = src._routes;
}

Server& Server::operator=(const Server& src) {
	if (this != &src) {
	_name = src._name;
	_host = src._host;
	_port = src._port;
	_maxPayload = src._maxPayload;
	_routes = src._routes;
	}
	return *this;
}

std::string						Server::getName() const { return this->_name; }
std::string						Server::getHost() const { return this->_host; }
int								Server::getPort() const { return this->_port; }
int								Server::getMaxPayload() { return _maxPayload; }
std::map<std::string, Route>&	Server::getRoutes() { return (std::map<std::string, Route>&)_routes; }
std::list<Socket>&				Server::getSocketList() { return _socketList; }

void	Server::setName(const std::string& name) { this->_name = name; }
void	Server::setHost(const std::string& host) { this->_host = host; }

void	Server::setPort(const std::string& port) {
	char	*err;
	int		portNumber;
	
	portNumber = strtol(port.c_str(), &err, 10);
	if (portNumber < MIN_PORT_NUMBER || portNumber > MAX_PORT_NUMBER || isalpha(*err)) {
		std::cerr << "Config file error: Invalid port number." << std::endl;
		exit(EXIT_FAILURE);
	}
	_port = portNumber;
}

void	Server::setMaxPayLoad(const std::string& maxPayLoad) {
	char	*err;
	int		payloadSize;
	
	payloadSize = strtol(maxPayLoad.c_str(), &err, 10);
	if (payloadSize < MIN_PAYLOAD || payloadSize > MAX_PAYLOAD || isalpha(*err)) {
		std::cerr << "Config file error: Ivalid payload size." << std::endl;
		exit(EXIT_FAILURE); //TODO terminate
	}
	_maxPayload = payloadSize;
}