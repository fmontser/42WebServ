#pragma once

#include <vector>
#include <map>
#include <poll.h>
#include "WebSocket.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class WebServer {
private:
	WebSocket&					_webSocket;
	std::vector<pollfd>			_pfdList;
public:
	WebServer(WebSocket& webSocket);
	~WebServer();
	void run();
private:
	int		acceptConnection();
	void	recieveData(int newSocketFd);
	void	sendData(int newSocketFd);
	void	processRequest(HttpRequest request);
};