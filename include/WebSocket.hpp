#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class WebSocket
{
private:
	int	_sockfd;
	int	_port;
	struct sockaddr_in _serv_addr;

public:
	WebSocket(int port);
	~WebSocket();
	void start();
	int getSocketFd();
};
