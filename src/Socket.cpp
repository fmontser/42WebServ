#include "Socket.hpp"

Socket::Socket(int port, struct pollfd pfd, SocketType type) : _port(port), _pollFd(pfd), _type(type) {}
Socket::~Socket() {}

Socket::Socket(const Socket& src) {
	_port = src._port;
	_pollFd = src._pollFd;
	_type = src._type;
}

Socket& Socket::operator=(const Socket& src) {
	if (this != &src){
		_port = src._port;
		_pollFd = src._pollFd;
		_type = src._type;
	}
	return *this;
}

bool Socket::operator==(const Socket& src) {
	return (_pollFd.fd == src._pollFd.fd) 
			&& (_type == src._type);
}

bool Socket::operator!=(const Socket& src) {
	return !(*this == src);
}

struct pollfd	Socket::getPollFd() const { return _pollFd; }
SocketType		Socket::getType() const { return _type; }
int				Socket::getPort() const { return _port; }

void			Socket::setPollFd(struct pollfd pollFd) { _pollFd = pollFd; }

bool	Socket::hasPollErr() const { return _pollFd.revents & POLLERR; }
bool	Socket::hasPollIn() const { return _pollFd.revents & POLLIN; }
bool	Socket::hasPollOut() const { return _pollFd.revents & POLLOUT; }