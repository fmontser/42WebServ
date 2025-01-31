#include <iostream>
#include <fstream>
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

static std::string getFileName(std::string url) {
	return url.substr(1, url.size() - 1);
}

//TODO @@@@@@ routes : el ultimo slash /, ha de ser normalizado?? .....

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


void	FileManager::processHttpRequest(Server& server) {
	if (validateRoute(server, _request)) {
		if (_request.getMethod() == "GET") {
			
			std::cout << "File is: " << getFileName(_request.getUrl()) << std::endl;
		}
		else if (_request.getMethod() == "POST") {

		}
		else if (_request.getMethod() == "DELETE") {

		}
		else {
			//TODO return method not suported.
			//_response.setVersion(HTTP_VERSION);
		}
	}
}



void	FileManager::recieveHttpRequest(Socket *targetSocket, HttpRequest& request) {
	_request = request;
	processHttpRequest(*targetSocket->getParentServer());
	DataAdapter::sendData(targetSocket, _response);
}

void	FileManager::recieveHttpResponse(Socket *targetSocket, HttpResponse& response) {
	_response = response;
	DataAdapter::sendData(targetSocket, _response);
}

bool FileManager::searchFile(Server& server, HttpRequest& _request) {
	std::map<std::string, Route>::iterator it;
	it = server.getRoutes().find(getRouteName(_request.getUrl()));
	if ( it != server.getRoutes().end()) {
		return true;
	}
	return false;
}

bool FileManager::createFile(Server& server, HttpRequest& _request) {
	std::map<std::string, Route>::iterator it;
	it = server.getRoutes().find(getRouteName(_request.getUrl()));
	if ( it == server.getRoutes().end()) {
		return true;
	}
	return false;
}

bool FileManager::deleteFile(Server& server, HttpRequest& _request) {
	std::map<std::string, Route>::iterator it;
	it = server.getRoutes().find(getRouteName(_request.getUrl()));
	if ( it != server.getRoutes().end()) {
		return true;
	}
	return false;
}

bool FileManager::controlAccess(Server& server, HttpRequest& _request) {
	std::map<std::string, Route>::iterator it;
	it = server.getRoutes().find(getRouteName(_request.getUrl()));
	if ( it != server.getRoutes().end() && it->second.getMethods().find(_request.getMethod()) != it->second.getMethods().end()) {
		return true;
	}
	return false;
}
