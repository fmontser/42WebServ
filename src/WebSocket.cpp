#include "WebSocket.hpp"
#include <sys/socket.h>
#include "colors.hpp"

#define SOCKET_BUFFER_SIZE 65536
#define SOCKET_LISTEN_QUEUE_SIZE 5

WebSocket::WebSocket(int port) :  _port(port) {
	int	optionEnable = 1;
	int optionBufferSize = SOCKET_BUFFER_SIZE;

	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		std::cerr << RED << "Error: opening socket" << END << std::endl;
		exit(1);
	}

	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optionEnable, sizeof(int)) < 0
	&&	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEPORT, &optionEnable, sizeof(int)) < 0
	&&	setsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &optionBufferSize, sizeof(int)) < 0
	&&	setsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &optionBufferSize, sizeof(int)) < 0) {
		std::cerr << RED << "Error: setting socket options" << END << std::endl;
		exit(1);
	}

	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	_serv_addr.sin_port = htons(_port);

	if (bind(_sockfd, (struct sockaddr *)&_serv_addr, sizeof(_serv_addr)) < 0) {
		std::cerr << RED << "Error: failed to bind address" << END	<<std::endl;
		exit(1);
	}
	start();
}

WebSocket::~WebSocket() {
	close(_sockfd);
}

void WebSocket::start() {
	if (listen(_sockfd, SOCKET_LISTEN_QUEUE_SIZE) < 0) {
		std::cerr << RED << "Error: socket listen failed" << END << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Listening on port: " << _port <<  END << " 🚀" << std::endl;
}

int WebSocket::getSocketFd() { return _sockfd; }
