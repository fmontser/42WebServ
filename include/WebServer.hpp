#pragma once

#include <list>
#include <map>
#include <poll.h>
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class WebServer {
private:
	Config&						_config;
	std::list<pollfd *>			_pollSockets;
public:
	WebServer(Config& config);
	~WebServer();
	void run();
private:
	int		acceptConnection();
	void	recieveData(int newSocketFd);
	void	sendData(int newSocketFd);
	void	processRequest(HttpRequest request);
};