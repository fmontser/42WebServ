#include "WebSocket.hpp"
#include "colors.hpp"

WebSocket::WebSocket(int port) : _sockfd(-1), _port(port) {
	// Create a socket
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd < 0) {
		std::cerr << RED << "Error opening socket" << END << std::endl;
		exit(1);
	}
	int opt = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		std::cerr << RED << "Error setting socket options" << END << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Socket created" << END << std::endl;
	// Initialize the server address
	memset((char *)&_serv_addr, 0, sizeof(_serv_addr));
	_serv_addr.sin_family = AF_INET;
	_serv_addr.sin_addr.s_addr = INADDR_ANY;
	_serv_addr.sin_port = htons(_port);
	// Bind the socket to the server address
	if (bind(_sockfd, (struct sockaddr *) &_serv_addr, sizeof(_serv_addr)) < 0) {
		std::cerr << RED << "Error on binding" << END	<<std::endl;
		exit(1);
	}
	std::cout << GREEN << "Socket bound to: " << _port << END << " ⏳" << std::endl;
	if (listen(_sockfd, SOMAXCONN) < 0) {
		std::cerr << RED << "Error on listening" << END << std::endl;
		exit(1);
	}
	std::cout << GREEN << "Listening on port: " << _port <<  END << " 🚀" << std::endl;
}

WebSocket::~WebSocket() {
	if (_sockfd >= 0)
		close(_sockfd);
	close(_sockfd);
}

void WebSocket::start() {
	if (_sockfd < 0) {
		std::cerr << RED << "Error opening socket" << END << std::endl;
		exit(1);
	}
	listen(_sockfd, 5);
}

void WebSocket::stop() {
	close(_sockfd);
}

int WebSocket::getServersocket() {
	return _sockfd;
}
