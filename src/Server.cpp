#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"

Server::Server() {}

Server::~Server() {
	close(_socketFd);
	for (std::list<Connection *>::iterator it = _connectionList.begin(); it != _connectionList.end(); ++it)
		delete (*it);
	_connectionList.clear();
}
Server::Server(const Server& src) {	
	_name = src._name;
	_host = src._host;
	_port = src._port;
	_root = src._root;
	_default = src._default;
	_maxPayload = src._maxPayload;
	_routes = src._routes;
	_connectionList = src._connectionList;
	_socketFd = src._socketFd;
	_pollfd = src._pollfd;
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
		_connectionList = src._connectionList;
		_socketFd = src._socketFd;
		_pollfd = src._pollfd;
		_method = src._method;
	}
	return *this;
}

void Server::listenSocket() {
	int optionEnable = 1;
	int optionBufferSize = SOCKET_BUFFER_SIZE;
	struct sockaddr_in address;

	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd < 0) {
		std::cerr << RED << "Error: opening socket" << END << std::endl;
		exit(1);
	}

	if (setsockopt(_socketFd, SOL_SOCKET, SO_REUSEADDR, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_REUSEPORT, &optionEnable, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_RCVBUF, &optionBufferSize, sizeof(int)) < 0
	&&  setsockopt(_socketFd, SOL_SOCKET, SO_SNDBUF, &optionBufferSize, sizeof(int)) < 0) {
		std::cerr << RED << "Error: setting socket options" << END << std::endl;
		exit(1);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_port);

	if (bind(_socketFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		std::cerr << RED << "Error: failed to bind address" << END << std::endl;
		exit(1);
	}

	if (listen(_socketFd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
		std::cerr << RED << "Error: socket listen failed" << END << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Listening on port: " << _port <<  END << " 🚀" << std::endl;

	_pollfd = pollfd();
	_pollfd.fd = _socketFd;
	_pollfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_pollfd.revents = 0;
}

std::string						Server::getName() const { return this->_name; }
std::string						Server::getHost() const { return this->_host; }
int										Server::getPort() const { return this->_port; }
std::string						Server::getRoot() const { return this->_root; }
int										Server::getMaxPayload() const { return _maxPayload; }


std::map<std::string, Route>&	Server::getRoutes() { return (std::map<std::string, Route>&)_routes; }
std::list<Connection *>&		Server::getConnectionList() { return _connectionList; }
int								Server::getSocketFd() const { return _socketFd; }
struct pollfd					Server::getPollfd() const { return _pollfd; }
std::string						Server::getDefault() const { return _default; }

void	Server::setName(const std::string& name) { this->_name = name; }
void	Server::setHost(const std::string& host) { this->_host = host; }
void	Server::setRoot(const std::string& root) {_root = root; }
void	Server::setDefault(const std::string& default_) {_default = default_; }
void	Server::setSocketFd(int socketFd) { _socketFd = socketFd; }
void	Server::setPollfd(struct pollfd pfd) {_pollfd = pfd; }
void	Server::addConfigMethods(const std::string& method) {_method[method] = "allowed"; }

void	Server::setPort(const std::string& port) {
	char	*err;
	int		portNumber;
	
	//TODO esta validacion deberia estar en config
	portNumber = strtol(port.c_str(), &err, 10);
	_port = portNumber;
}

void	Server::setMaxPayLoad(const std::string& maxPayLoad) {
	char	*err;
	int		payloadSize;
	
	//TODO esta validacion deberia estar en config
	payloadSize = strtol(maxPayLoad.c_str(), &err, 10);
	_maxPayload = payloadSize;
}

bool	Server::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool	Server::hasPollOut() const { return _pollfd.revents && POLLOUT; }