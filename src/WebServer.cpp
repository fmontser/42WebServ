#include "WebServer.hpp"
#include "colors.hpp"
#include <algorithm>

// WebServer::WebServer(WebSocket& ws) : _ws(ws) {
// 	struct pollfd pfd;
// 	pfd.fd = _ws.getServersocket();
// 	pfd.events = POLLIN;
// 	pfd.revents = 0;
// 	_fds.push_back(pfd);
// }

WebServer::WebServer(WebSocket& ws) : _ws(ws) {
	// pollfd pfd;
	// pfd = { _ws.getServersocket(), POLLIN, 0 };
	// pfd.push_back(pfd);
}

WebServer::~WebServer() {
	_ws.stop();
}

void WebServer::run() {
	pollfd pfd = { _ws.getServersocket(), POLLIN, 0 };
	_fds.push_back(pfd);
	std::cout << GREEN << "Starting webserver loop..." << END << std::endl;
	
	while (true) {
		int ret = poll(_fds.data(), _fds.size(), -1);
		if (ret == -1) {
			std::cerr << RED << "poll() failed" << END << std::endl;
			break;
		}
		if (ret == 0) {
			std::cerr << RED << "poll() timed out" << END << std::endl;
			continue;
		}
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _ws.getServersocket()) {
					_acceptConnection();
				} else {
					_handleClient(_fds[i]);
				}
			}
		}
	}
}

void	WebServer::_acceptConnection() {
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int newSocket = accept(_ws.getServersocket(), (sockaddr*)&client_addr, &client_addr_len);
	if (newSocket == -1) {
		std::cerr << RED << "Error accepting new connection " << END << "࣬࣬ 👾" << std::endl;
		return;
	}
	std::cout << GREEN << "New connection, socket fd: " << newSocket << END << std::endl;
	// int clientfd = accept(_ws.getServersocket(), NULL, NULL);
	// if (clientfd == -1) {
	// 	std::cerr << RED << "accept() failed" << END << std::endl;
	// 	return;
	// }
	// std::cout << GREEN << "New connection" << END << std::endl;
	// pollfd pfd = { clientfd, POLLIN, 0 };
	// _fds.push_back(pfd);
}
struct RemovePollfd {
		int fdToRemove;
		RemovePollfd(int fd) : fdToRemove(fd) {}
		bool operator()(const pollfd& p) const {
				return p.fd == fdToRemove;
	}
};

void	WebServer::_handleClient(pollfd& pfd) {
	char buf[1024] = {0};
	int len = recv(pfd.fd, buf, sizeof(buf), 0);
	if (len == -1) {
		std::cerr << RED << "Client disconnected, socket fd: " << pfd.fd << END << std::endl;
		close(pfd.fd);
		_fds.erase(std::remove_if(_fds.begin(), _fds.end(), RemovePollfd(pfd.fd)), _fds.end()); 

		_clients.erase(pfd.fd);
		return;
	} else if (len == 0) {
		std::cerr << RED << "recv() failed" << END << std::endl;
		return;
	} else {
		std::cout << BLUE << "Received: " << buf << END << std::endl;
		_clients[pfd.fd] += buf;
	}


//	buf[len] = '\0';
//	std::cout << BLUE << "Received: " << buf << END << std::endl;
}