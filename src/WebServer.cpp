#include "WebServer.hpp"
#include "colors.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define TIMEOUT 10000
#define BUFFER_SIZE 8192
#define HTTP_VERSION "HTTP/1.1"

WebServer::WebServer(Config& config) : _config(config) {

	_pollSockets.push_back(new pollfd());

	_pollSockets.back()->fd = _config.getWebSocket().getSocketFd();
	_pollSockets.back()->events = POLLIN | POLLOUT | POLLERR;
	_pollSockets.back()->revents = 0;

	run();
}

WebServer::~WebServer() {
	for (std::list<pollfd *>::iterator it = _pollSockets.begin(); it != _pollSockets.end(); ++it) {
		close((*it)->fd);
		delete *it;
	}
	_pollSockets.clear();
}

void WebServer::run() {
	int		pollStatus;
	int		newSocketFd;

	while (true) {
		size_t arraySize = 0;
		pollfd socketArray[_pollSockets.size()];
		for (std::list<pollfd *>::iterator it = _pollSockets.begin(); it != _pollSockets.end(); ++it) {
			socketArray[arraySize++] = *(*it);
		}
		pollStatus = poll(socketArray, _pollSockets.size(), TIMEOUT);
		if (pollStatus == -1) {
			//TODO gestionar error de socket
			std::cerr << RED << "Error: WebSocket error" << END << std::endl;
		}
		else if (pollStatus > 0) {
			for (size_t i = 0; i < arraySize; i++)
			{
				if (socketArray[i].revents & POLLIN) {

					if (socketArray[i].fd == _config.getWebSocket().getSocketFd()) {
						newSocketFd = acceptConnection();
						_pollSockets.push_back(new pollfd());
						_pollSockets.back()->fd = newSocketFd;
						_pollSockets.back()->events = POLLIN | POLLOUT | POLLERR | POLLHUP;
						_pollSockets.back()->revents = 0;
						recieveData(newSocketFd);
					}
					else
						recieveData(socketArray[i].fd);
				}
			}
		}
	}
}

int	WebServer::acceptConnection() {
	sockaddr_in	client_addr;
	socklen_t	client_addr_len = sizeof(client_addr);

	int newSocketFd = accept(_config.getWebSocket().getSocketFd(), (sockaddr *)&client_addr, &client_addr_len);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
	}
	return newSocketFd;
}

void	WebServer::recieveData(int socketFd) {
	char buf[BUFFER_SIZE] = {0};

	int len = recv(socketFd, buf, BUFFER_SIZE, 0);
	if (len == -1) {
		std::cerr << RED << "Error: client error " << socketFd << END << std::endl;
	}
	else if (len > 0) {
		HttpRequest request;

		std::stringstream sstream(buf);
		request.pull(&sstream, socketFd);
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
		std::string body;
		std::stringstream sstream;
		std::fstream index("../web/index.html", std::ios::in);
		std::fstream style("../web/style.css", std::ios::in);

		if (request.getUrl() == "/") {
			if (!index) {
				std::cerr << "File error: cannot access file" << std::endl; //TODO 404?
				return;
			}
			body.assign((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
			response.addHeader("Content-Type", "text/html");
		}
		else if (request.getUrl() == "/style.css") {
			if (!style) {
				std::cerr << "File error: cannot access file" << std::endl; //TODO 404?
				return;
			}
			body.assign((std::istreambuf_iterator<char>(style)), std::istreambuf_iterator<char>());
			response.addHeader("Content-Type", "text/css");
		}		
		
		sstream << std::strlen(body.c_str());

		response.setVersion(HTTP_VERSION);
		response.setStatusCode("200");
		response.setStatusMsg("OK");
		response.addHeader("Content-Length", sstream.str());
		response.addHeader("Connection", "keep-alive");
		//response.addHeader("Date", "Fri, 17 Jan 2025 10:00:00 GMT"); //opcional!
		response.setBody(body);
		response.push(request.getSocket());
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