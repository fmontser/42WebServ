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
	int _sockfd;
	int _port;
	//la estructura sockaddr_in contiene un sin_family, un sin_port y un sin_addr 
	//que son la familia de direcciones, el puerto y la dirección IP del servidor
	//respectivamente. Esta estructura se utiliza para almacenar la dirección del
	//servidor y es una funcion de la libreria de sockets.
	struct sockaddr_in _serv_addr;

public:
	WebSocket(int port);
	~WebSocket();
	void start();
	void stop();
	int getServersocket();

};
