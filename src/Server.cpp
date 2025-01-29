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

std::string	Server::getName() const { return this->_name; }
std::string	Server::getHost() const { return this->_host; }
int			Server::getPort() const { return this->_port; }

void Server::setName(const std::string& name) { this->_name = name; }
void Server::setHost(const std::string& host) { this->_host = host; }

void Server::setPort(const std::string& port) {
	char	*err;
	int		portNumber;
	
	portNumber = strtol(port.c_str(), &err, 10);
	if (portNumber < MIN_PORT_NUMBER || portNumber > MAX_PORT_NUMBER || isalpha(*err)) {
		std::cerr << "Config file error: Invalid port number." << std::endl;
		exit(EXIT_FAILURE);
	}
	_port = portNumber;
}
