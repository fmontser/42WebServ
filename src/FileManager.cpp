#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include "Config.hpp"
#include "FileManager.hpp"
#include "DataAdapter.hpp"
#include "ServerConstants.hpp"

HttpRequest		FileManager::_request;
HttpResponse	FileManager::_response;

FileManager::FileManager() {}
FileManager::~FileManager() {}

FileManager::FileManager(const FileManager& src) {
	_request = src._request;
	_response = src._response;
}

FileManager& FileManager::operator=(const FileManager& src) {
	if (this != &src) {
		_request = src._request;
		_response = src._response;
	}
	return *this;
}

static bool isDirectory(const std::string& url) {
	return (url.substr(url.size() - 1, 1) == "/") ? true : false;
}

static void chunkEncode(std::string& body, size_t maxPayload) {
	std::stringstream	buffer;

	while (body.size()) {
		int	csize = body.size() >= maxPayload ? maxPayload : body.size();
		buffer << std::hex << csize << CRLF;
		buffer << body.substr(0, csize) << CRLF;
		body = body.substr(csize, body.size());
	}
	buffer << '0' << CRLF << CRLF;
	body = buffer.str();
}

static int readFile(Socket *socket, HttpRequest& request, HttpResponse& response ) {
	std::string			target, body; 
	int					fd, readSize;
	Server&	server = *socket->getParentServer();

	target = server.getRoot().substr(0, server.getRoot().size() - 1).append(request.getUrl());
	if (isDirectory(request.getUrl()))
		target.append("index.html"); //TODO respuesta default si se solicita un directorio, que pasa con index.php?
	
	fd = open(target.c_str(), O_RDONLY, 0644);
	if (fd < 0) {
		fd = open("../web/default/404.html", O_RDONLY, 0644); //TODO hardcoded, debe obtener la ruta del config.
		response.setStatusCode("404");
		response.setStatusMsg("NOT_FOUND");
		std::cerr << YELLOW << "Warning: error 404 \"" << target << "\", NOT_FOUND " << END << std::endl;
	}
	do {
		char readBuffer[READ_BUFFER] = {0};
		readSize = read(fd, readBuffer, READ_BUFFER);
		body.append(readBuffer);
	} while (readSize > 0);
	
	if ((int)body.size() > server.getMaxPayload()){
		response.addHeader(std::make_pair("Transfer-Encoding","chunked"));
		chunkEncode(body, server.getMaxPayload());
	}
	else {
		std::stringstream	bodySize;
		bodySize << body.size() - 1;
		response.addHeader(std::make_pair("Content-Length", bodySize.str()));
	}
	response.setBody(body);
	return (fd);
}

static int writeFile(Socket *socket , HttpRequest& request, HttpResponse& response ) {
	//TODO @@@@@@@ implementar escritura del archivo...hayq que quitar las boundaries
	(void)socket;
	(void)request;
	(void)response;

	//envia el cliente todo por el mismo socket??? 
	return 0;
}

void	FileManager::processMultiPart(Socket *socket) {
	int fd;

	if (socket->multiMode) {
		_request.setBody(socket->recvBuffer);
		socket->multiMode = false;
	}
	
	fd = writeFile(socket, _request, _response);
	if (fd < 0)
		(void)socket; //TODO error 500!
	_response.setStatusCode("201");
	_response.setStatusMsg("CREATED");
	std::cout << BLUE << "Info: success 201 \"" << _request.getMethod() << "\", CREATED " << END << std::endl;

}

void	FileManager::processHttpRequest(Socket *socket) {
	int	fd;

	_response.setVersion(HTTP_VERSION);
	_response.addHeader(std::make_pair("Connection", "keep-alive"));
	if (_request.getMethod() == "GET") {
		fd = readFile(socket, _request, _response);
		if (_response.getStatusCode().empty()) {
			_response.setStatusCode("200");
			_response.setStatusMsg("OK");
			std::cout << BLUE << "Info: success 200 \"" << _request.getMethod() << "\", OK " << " FD: " << socket->getFd() << END << std::endl;
		}
	}
	else if (_request.getMethod() == "POST") {

		//TODO set boundarie....hardcoded
		if (socket->boundarie.empty()) {
			std::string bound = _request.getHeaders().find("Content-Type")->second;
			std::string len = _request.getHeaders().find("Content-Length")->second;
			socket->contentLength = atoi(len.c_str());
			socket->boundarie = bound.substr(bound.find('=') + 1, bound.size());
			socket->multiMode = true;
		}


	}
	else if (_request.getMethod() == "DELETE") {
		//TODO DELETE METHOD
		_response.setStatusCode("204");
		_response.setStatusMsg("NO_CONTENT");
		std::cout << BLUE << "Info: success 204 \"" << _request.getMethod() << "\", NO_CONTENT " << END << std::endl;
	}
	else {
/* 		_request.setUrl("/default/501.html"); //TODO hardcoded, debe obtener la ruta del config.
		fd = readFile(socket, _request, _response);
		_response.setStatusCode("501");
		_response.setStatusMsg("METHOD_NOT_IMPLEMENTED");
		std::cerr << YELLOW << "Warning: Error 501 \"" << _request.getMethod() << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl; */
	}
	if (fd > 2)
		close(fd);
}

void	FileManager::recieveHttpRequest(Socket *socket, HttpRequest& request) {
	_request = request;
	processHttpRequest(socket);
	//TODO multipart no responde entre partes...
 	if (!socket->multiMode)
		DataAdapter::sendData(socket, _response);
	_request.clear();
	_response.clear();
}

void	FileManager::recieveHttpResponse(Socket *socket, HttpResponse& response) {
	_response = response;
	DataAdapter::sendData(socket, _response);
	_request.clear();
	_response.clear();
}
