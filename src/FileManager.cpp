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

