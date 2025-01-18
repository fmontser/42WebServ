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
				//TODO es posible escribir
			}
			if (_pfdList[1].revents & POLLIN) { //TODO hay que pasar por la lista...
				recieveData(_pfdList[1].fd);
			}
			if (_pfdList[1].revents & POLLOUT) {
				//TODO es posible escribir
			}
			if (_pfdList[1].revents & POLLHUP) { //TODO harcoded...
				close(_pfdList[1].fd);
				_pfdList.pop_back(); //TODO pffff....
			}
		}
	}
}

int	WebServer::acceptConnection() {
	pollfd		newClientSocket;
	sockaddr_in	clientAddress;
	socklen_t	clientAddressLength = sizeof(clientAddress);
	
	int newSocketFd = accept(_webSocket.getSocketFd(), (sockaddr *)&clientAddress, &clientAddressLength);
	if (newSocketFd == -1) {
		std::cerr << RED << "Error: connection failed " << END << "࣬࣬ 👾" << std::endl;
	}

	newClientSocket.fd = newSocketFd;
	newClientSocket.events = POLLIN | POLLERR | POLLHUP;
	newClientSocket.revents = 0;

	_pfdList.push_back(newClientSocket); //TODO al cerrar la conexion hay que limpar esto!!!
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
		response.addHeader("Date", "Fri, 17 Jan 2025 10:00:00 GMT");
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