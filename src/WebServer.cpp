#include "WebServer.hpp"
#include "colors.hpp"
#include <fstream>
#include <sstream>
#include <string>

#define TIMEOUT 1000
#define BUFFER_SIZE 8192
#define HTTP_VERSION "HTTP/1.1"

WebServer::WebServer(WebSocket& webSocket) : _webSocket(webSocket) {
	pollfd pfdWebSocket;

	pfdWebSocket.fd = webSocket.getSocketFd();
	pfdWebSocket.events = POLLIN | POLLOUT | POLLERR;
	pfdWebSocket.revents = 0;

	_pfdList.push_back(pfdWebSocket);
	run();
}

WebServer::~WebServer() {}

void WebServer::run() {
	int pollValue;
	
	while (true) {
		pollValue = poll(_pfdList.data(), _pfdList.size(), TIMEOUT);
		if (pollValue == -1) {
			//TODO gestionar error de socket
			std::cerr << RED << "Error: WebSocket error" << END << std::endl;
		}
		else if (pollValue > 0) {
			if (_pfdList[0].revents & POLLIN) {
				int newSocketFd = acceptConnection();
				recieveData(newSocketFd);
			}
			if (_pfdList[0].revents & POLLOUT) {
				acceptConnection();
				//TODO es posible escribir
			}
		}
	}
}

int	WebServer::acceptConnection() {
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	
	int newSocketFd = accept(_webSocket.getSocketFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
	}
	return newSocketFd;
}

void	WebServer::recieveData(int newSocketFd) {
	char buf[BUFFER_SIZE] = {0};

	int len = recv(newSocketFd, buf, BUFFER_SIZE, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << newSocketFd << END << std::endl;
	}
	else if (len > 0) {
		HttpRequest request;

		std::stringstream sstream(buf);
		request.pull(&sstream);
		processRequest(request);
	}
}

void	WebServer::sendData(int newSocketFd) {
	(void)newSocketFd;
	//TODO send!
}

void WebServer::processRequest(HttpRequest request){
	if (request.getVersion() != HTTP_VERSION )
		std::cerr << "Error: wrong version" << std::endl;
	//TODO ahcer un map con funciones??

	if (request.getMethod() == "GET"){
		HttpResponse response;
		std::string line;
		std::string body;
		std::fstream file("../web/index.html", std::ios::in);

		while (std::getline(file, line)){
			body.append(line);
			body.append("\n");
		}
		
		response.setVersion(HTTP_VERSION);
		response.setStatusCode("200");
		response.setStatusMsg("OK");
		response.addHeader("Date", "17/01/2025");
		response.setBody(body);
		response.push();


	}
	else if (request.getMethod() == "PUT"){
		//TODO
	}
	else if (request.getMethod() == "DELETE"){
		//TODO
	}
	else {
		//TODO
	}
}