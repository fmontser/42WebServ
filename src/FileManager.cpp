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


void	FileManager::processHttpRequest(Server& server) {
	std::string	target = server.getRoot().substr(0, server.getRoot().size() - 1).append(_request.getUrl());
	std::string	body;
	std::stringstream bodySize;
	std::multimap<std::string, std::string>::iterator it;
	int			fd;
	int			readSize;

	if (_request.getMethod() == "GET") {
		if (isDirectory(_request.getUrl()))
			target.append("index.html"); //Default
		fd = open(target.c_str(), O_RDONLY, 0644);
		if (fd < 0) {
			//TODO 404!!
			std::cerr << "404!!!" << std::endl;
		}

		do {
			char readBuffer[1] = {0};  //TODO cambiar buffer al buffer del servidor
			readSize = read(fd, readBuffer, 1);  //TODO cambiar buffer al buffer del servidor
			body.append(readBuffer);

		} while (readSize > 0);
		bodySize << body.size() - 1;

		
		_response.setVersion(HTTP_VERSION);
		_response.setStatusCode("200");
		_response.setStatusMsg("OK");
		_response.addHeader(std::make_pair("Content-Length", bodySize.str()));
		_response.setBody(body);
		



/* 		if (_request.getUrl() == "/favicon.png")
			_response.addHeader(std::make_pair("Content-Type", "image/png")); */

		//TODO @@@@@@ content-type usando un mapa por tipos de archivo??...
	}
	else if (_request.getMethod() == "POST") {
		//TODO
	}
	else if (_request.getMethod() == "DELETE") {

	}
	else {
		//TODO return method not suported.
		//_response.setVersion(HTTP_VERSION);
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
