#include <iostream>
#include "HttpHeader.hpp"
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"


//TODO @@@@@@@@@@ continuar manejar rutas!!!!!
static	bool	validateRouteMethod(std::string method, Server& server, HttpRequest& request) {
/* 
	Route route =  != ;
	std::map<std::string, Route>::iterator routeIt = server.getRoutes().find(request.url);
	if (routeIt != server.getRoutes().end())
		return true;
	return false; */
	return true;
}

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();

	if (!validateRouteMethod(request.method, connection->getServer())) {
		response.setupResponse(HttpResponse::METHOD_NOT_ALLOWED, dataAdapter);
		return ;
	}
	if (request.method == "GET") {
		rtype  = FileManager::readFile(dataAdapter);
		response.setupResponse(rtype, dataAdapter);
		connection->isChunkedResponse = response.isChunked();
	}
	else if (request.method == "POST") {
		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.setupResponse(HttpResponse::CONTINUE, dataAdapter);
			return;
		}
		HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter);
		if (connection->contentLength == 0)
			response.setupResponse(rtype, dataAdapter);
	}
	else if (request.method == "DELETE") {
		rtype = FileManager::deleteFile(dataAdapter);
		response.setupResponse(rtype, dataAdapter);
	}
	else {
		response.setupResponse(HttpResponse::METHOD_NOT_IMPLEMENTED, dataAdapter);
	}
	
}


