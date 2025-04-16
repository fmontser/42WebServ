#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
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
	_defaults["default500"] = "defaults/500.html";
	_defaults["default501"] = "defaults/501.html";
}

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
	_defaults = src._defaults;
	_maxPayload = src._maxPayload;
	_routes = src._routes;
	_connectionList = src._connectionList;
	_socketFd = src._socketFd;
	_pollfd = src._pollfd;
	_serverMethods = src._serverMethods;
}

Server& Server::operator=(const Server& src) {
	if (this != &src) {
		_name = src._name;
		_host = src._host;
		_port = src._port;
		_root = src._root;
		_defaults = src._defaults;
		_maxPayload = src._maxPayload;
		_routes = src._routes;
		_connectionList = src._connectionList;
		_socketFd = src._socketFd;
		_pollfd = src._pollfd;
		_serverMethods = src._serverMethods;
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
std::vector<std::string>&		Server::getServerMethods() { return this->_serverMethods; }
std::map<std::string, std::string>&		Server::getDefaults() {return this->_defaults; }
size_t										Server::getMaxPayload() const { return _maxPayload; }


std::map<std::string, Route>&	Server::getRoutes() { return _routes; }
std::list<Connection *>&		Server::getConnectionList() { return _connectionList; }
int								Server::getSocketFd() const { return _socketFd; }
struct pollfd					Server::getPollfd() const { return _pollfd; }


void	Server::setName(const std::string& name) { this->_name = name; }
void	Server::setHost(const std::string& host) { this->_host = host; }
void	Server::setRoot(const std::string& root) {_root = root; }
void	Server::setServerMethods(const std::vector<std::string>& serverMethods) { _serverMethods = serverMethods; }
void	Server::setSocketFd(int socketFd) { _socketFd = socketFd; }
void	Server::setPollfd(struct pollfd pfd) {_pollfd = pfd; }

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


Route	*Server::getRequestedRoute(DataAdapter& dataAdapter) {
	std::string path, url;

	url = dataAdapter.getRequest().url;
	if (dataAdapter.getRequest().isCgiRequest)
		url = CgiAdapter::stripCgiParams(url);

	path = PathManager::resolveServerPath(dataAdapter);

	if (!Utils::isDirectory(path)) {
		if (!(access(path.c_str(), F_OK) == 0))
			return NULL;
		url = Utils::getUrlPath(url);
	}

	std::map<std::string, Route>::iterator it = _routes.find(url);
	if (it !=  _routes.end())
		return &it->second;

	it = _routes.find(url.append("/"));
	if (it !=  _routes.end())
		return &it->second;
	
	return NULL;
}

bool	Server::hasPollIn() const { return _pollfd.revents & POLLIN; }
bool	Server::hasPollOut() const { return _pollfd.revents && POLLOUT; }