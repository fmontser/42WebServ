#include "ServerConstants.hpp"
#include "Socket.hpp"
#include <iostream>
#include <sys/socket.h>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>

Socket::Socket() {}

Socket::~Socket() {
	close(_fd);
}

Socket::Socket(const Socket& src) {
		_port = src._port;
		_fd = src._fd; //dup(src._fd);
		_parentServer = src._parentServer;
		_serverFlag = src._serverFlag;
		_pollfd = src._pollfd;
		_pollfd.fd = _fd;
		sendBuffer = src.sendBuffer;
}

Socket& Socket::operator=(const Socket& src) {
	if (this != &src) {
		_port = src._port;
		_fd = src._fd; //dup(src._fd);
		_parentServer = src._parentServer;
		_serverFlag = src._serverFlag;
		_pollfd = src._pollfd;
		_pollfd.fd = _fd;
		sendBuffer = src.sendBuffer;
	}
	return *this;
}

bool Socket::operator==(const Socket& src) {
	return _fd == src._fd;
}

void Socket::enableSocket(bool serverFlag) {
	int optionEnable = 1;
	int optionBufferSize = SOCKET_BUFFER_SIZE; //TODO payload_
	struct sockaddr_in address;

	_serverFlag = serverFlag;
	if (_serverFlag) {
				_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (_fd < 0) {
			std::cerr << RED << "Error: opening socket" << END << std::endl;
			exit(1);
		}

		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optionEnable, sizeof(int)) < 0
		&&  setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &optionEnable, sizeof(int)) < 0
		&&  setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &optionBufferSize, sizeof(int)) < 0
		&&  setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &optionBufferSize, sizeof(int)) < 0) {
			std::cerr << RED << "Error: setting socket options" << END << std::endl;
			exit(1);
		}

		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(_port);

		if (bind(_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
			std::cerr << RED << "Error: failed to bind address" << END << std::endl;
			exit(1);
		}

		if (listen(_fd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
			std::cerr << RED << "Error: socket listen failed" << END << std::endl;
			exit(1);
		}
		std::cout << GREEN << "Listening on port: " << _port <<  END << " 🚀" << std::endl;
	}

	_pollfd = pollfd();
	_pollfd.fd = _fd;
	_pollfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_pollfd.revents = 0;
}

unsigned int			Socket::getPort() const { return _port; }
int						Socket::getFd() const { return _fd; }
const struct pollfd&	Socket::getPollFd() const {	return _pollfd; }
bool					Socket::getServerFlag() const { return _serverFlag; }
Server					*Socket::getParentServer() const { return _parentServer; }

void					Socket::setPort(unsigned int port) { _port = port; }
void					Socket::setFd(int fd) { _fd = fd; }
void					Socket::setParentServer(Server *parentServer) { _parentServer = parentServer;}
void					Socket::updatePollFd(struct pollfd pfd) { _pollfd = pfd; }
