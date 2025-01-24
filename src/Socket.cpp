#include "Socket.hpp"
#include "Colors.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SOCKET_BUFFER_SIZE 65536
#define SOCKET_LISTEN_QUEUE_SIZE 5

Socket::Socket(int port) : _port(port) {
    configureSocket();
    start();
}

Socket::~Socket() {
    if (_fd > 0) {
        close(_fd);
    }
}
/* //Optional copy constructor .. I prefer the second one
Socket::Socket(const Socket& src) {
        this->_port = src._port;
        this->_fd = src._fd;
}
*/
Socket::Socket(const Socket& src) : _port(src._port), _fd(src._fd) {}
Socket& Socket::operator=(const Socket& src) {
    if (this != &src) {
        this->_port = src._port;
        this->_fd = src._fd;
    }
    return *this;
}

void Socket::configureSocket() {
     int optionEnable = 1;
    int optionBufferSize = SOCKET_BUFFER_SIZE;

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

    _serv_addr.sin_family = AF_INET;
    _serv_addr.sin_addr.s_addr = INADDR_ANY;
    _serv_addr.sin_port = htons(_port);

    if (bind(_fd, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) < 0) {
        std::cerr << RED << "Error: failed to bind address" << END << std::endl;
        exit(1);
    }
}

void Socket::start() {
    if (listen(_fd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
        std::cerr << RED << "Error: socket listen failed" << END << std::endl;
        exit(1);
    }
    std::cout << GREEN << "Listening on port: " << _port <<  END << " 🚀" << std::endl;
}

int Socket::getFd() const {
    return _fd;
}
unsigned int Socket::getPort() const {
    return _port;
}

void Socket::setPort(unsigned int port) {
    _port = port;
}