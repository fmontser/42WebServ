#include <iostream>
#include "HttpHeader.hpp"
#include "HttpProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"
#include "Utils.hpp"



static	HttpResponse::responseType	validateRouteMethod(HttpRequest& request, Route *actualRoute) {
	if(actualRoute == NULL)
		return HttpResponse::NOT_FOUND;
	if (!actualRoute->isMethodAllowed(request.method))
		return HttpResponse::METHOD_NOT_ALLOWED;
	return HttpResponse::EMPTY;
}

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();
	Route			*actualRoute = connection->getServer().getRequestedRoute(Utils::getUrlPath(request.url));

	rtype = validateRouteMethod(request, actualRoute);
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
		rtype  = FileManager::readFile(dataAdapter, actualRoute);
		response.setupResponse(rtype, dataAdapter);
		connection->isChunkedResponse = response.isChunked();
	}
	else if (request.method == "POST") {
		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.setupResponse(HttpResponse::CONTINUE, dataAdapter);
			return;
		}
		HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter, actualRoute);
		if (connection->contentLength == 0)
			response.setupResponse(rtype, dataAdapter);
	}
	else if (request.method == "DELETE") {
		rtype = FileManager::deleteFile(dataAdapter, actualRoute);
		response.setupResponse(rtype, dataAdapter);
	}
	else {
		response.setupResponse(HttpResponse::METHOD_NOT_IMPLEMENTED, dataAdapter);
	}
	
}


