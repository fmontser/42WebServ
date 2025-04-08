#include <iostream>
#include "HttpHeader.hpp"
#include "HttpProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"
#include "Utils.hpp"
#include <unistd.h>

static bool checkListingConditions(DataAdapter& adapter, Route *actualRoute) {
	HttpRequest	request = adapter.getRequest();
	std::string	path("..");
	std::string _default;

	path.append(adapter.getConnection()->getServer().getRoot());
	path.append(request.url);
	_default = std::string(path).append(actualRoute->getDefault());

	if (Utils::isDirectory(path)
		&& actualRoute->getAutoIndex() == "on"
		&& (_default.empty() || (access(actualRoute->getDefault().c_str(), F_OK != 0))))
			return true;
	return false;
}

static	HttpResponse::responseType	validateRouteMethod(DataAdapter& dataAdapter, Route *actualRoute) {
	if(actualRoute == NULL)
		return HttpResponse::NOT_FOUND;
	if (!actualRoute->isMethodAllowed(dataAdapter.getRequest().method))
		return HttpResponse::METHOD_NOT_ALLOWED;
	if (checkListingConditions(dataAdapter, actualRoute)) {
		return HttpResponse::DIR_LIST;
	}
	return HttpResponse::EMPTY;
}

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();
	Route			*actualRoute = connection->getServer().getRequestedRoute(Utils::getUrlPath(request.url));

	rtype = validateRouteMethod(dataAdapter, actualRoute);
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


