#include <iostream>
#include "HttpHeader.hpp"
#include "HttpProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"
#include "PathManager.hpp"
#include "Utils.hpp"
#include "Index.hpp"
#include <unistd.h>



static	HttpResponse::responseType	validateRouteMethod(DataAdapter& dataAdapter) {
	HttpRequest	request = dataAdapter.getRequest();
	Route	*actualRoute = dataAdapter.getConnection()
			->getServer().getRequestedRoute(dataAdapter);

	if(actualRoute == NULL)
		return HttpResponse::NOT_FOUND; 
	if (!actualRoute->isMethodAllowed(request.method))
		return HttpResponse::METHOD_NOT_ALLOWED;
	if (Index::isIndexRoute(dataAdapter, actualRoute)) {
		return HttpResponse::DIR_LIST;
	}
	return HttpResponse::EMPTY;
}

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();

	rtype = validateRouteMethod(dataAdapter);
	if (rtype != HttpResponse::EMPTY) {
		response.setupResponse(rtype, dataAdapter);
		return;
	}

	if (connection->isOverPayloadLimit) {
		connection->requestMode = Connection::SINGLE;
		response.setupResponse(HttpResponse::PAYLOAD_TOO_LARGE, dataAdapter);
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


