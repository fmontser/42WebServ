#include <iostream>
#include <fcntl.h>
#include <unistd.h>
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

static int readFile(std::string rootUrl, HttpRequest& request, HttpResponse& response ) {
	std::string			target, body; 
	std::stringstream	bodySize;
	int					fd, readSize;

	target = rootUrl.substr(0, rootUrl.size() - 1).append(request.getUrl());
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
	bodySize << body.size() - 1;
	
	response.addHeader(std::make_pair("Content-Length", bodySize.str()));
	response.setBody(body);
	return (fd);
}

void	FileManager::processHttpRequest(Server& server) {
	int	fd;

	_response.setVersion(HTTP_VERSION);
	if (_request.getMethod() == "GET") {
		fd = readFile(server.getRoot(), _request, _response);
		_response.setStatusCode("200");
		_response.setStatusMsg("OK");
		std::cout << BLUE << "Info: success 200 \"" << _request.getMethod() << "\", OK " << END << std::endl;
	}
	else if (_request.getMethod() == "POST") {
		//TODO POST METHOD
		_response.setStatusCode("201");
		_response.setStatusMsg("CREATED");
		std::cout << BLUE << "Info: success 201 \"" << _request.getMethod() << "\", CREATED " << END << std::endl;
	}
	else if (_request.getMethod() == "DELETE") {
		//TODO DELETE METHOD
		_response.setStatusCode("204");
		_response.setStatusMsg("NO_CONTENT");
		std::cout << BLUE << "Info: success 204 \"" << _request.getMethod() << "\", NO_CONTENT " << END << std::endl;
	}
	else {
		_request.setUrl("/default/501.html"); //TODO hardcoded, debe obtener la ruta del config.
		fd = readFile(server.getRoot(), _request, _response);
		_response.setStatusCode("501");
		_response.setStatusMsg("METHOD_NOT_IMPLEMENTED");
		std::cerr << YELLOW << "Warning: Error 501 \"" << _request.getMethod() << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl;
	}
	close(fd);
}

void	FileManager::recieveHttpRequest(Socket *targetSocket, HttpRequest& request) {
	_request = request;
	processHttpRequest(*targetSocket->getParentServer());
	DataAdapter::sendData(targetSocket, _response);
	_request.clear();
	_response.clear();
}

void	FileManager::recieveHttpResponse(Socket *targetSocket, HttpResponse& response) {
	_response = response;
	DataAdapter::sendData(targetSocket, _response);
	_request.clear();
	_response.clear();
}


/*



static std::string getFileName(std::string url) {
	return url.substr(1, url.size() - 1);
}


static std::string getRouteName(std::string url) {
	return url.substr(0, url.find_last_of('/', 0) - 1);
}

static bool validateRoute(Server& server, HttpRequest& request) {
	std::map<std::string, Route>::iterator it;
	it = server.getRoutes().find(getRouteName(request.getUrl()));
	if ( it != server.getRoutes().end()) {
		if (it->second.getMethods().find(request.getMethod()) != it->second.getMethods().end())
			return true;
	}
	return false;
}




*/
