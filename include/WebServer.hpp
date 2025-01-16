#pragma once

#include <vector>
#include <map>
#include <poll.h>
#include "WebSocket.hpp"

class WebServer {
private:
	WebSocket& _ws;
	std::vector<pollfd> _fds;
	std::map<int, std::string> _clients;
public:
	WebServer(WebSocket& ws);
	~WebServer();
	void run();
private:
	void _acceptConnection();
	void _handleClient(pollfd& client_fd);
};