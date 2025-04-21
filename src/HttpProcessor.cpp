#include <iostream>
#include "HttpHeader.hpp"
#include "HttpProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"
#include "PathManager.hpp"
#include "Utils.hpp"
#include "Index.hpp"
#include "CgiAdapter.hpp"
#include <unistd.h>

static	HttpResponse::responseType	validateRoute(DataAdapter& dataAdapter) {
	HttpRequest	request = dataAdapter.getRequest();
	Route	*actualRoute = dataAdapter.getConnection()
			->getServer().getRequestedRoute(dataAdapter);

	if(actualRoute == NULL)
		return HttpResponse::NOT_FOUND;
	if (!actualRoute->getRedirect().empty())
		return HttpResponse::SEE_OTHER;
	if (!actualRoute->isMethodAllowed(request.method))
		return HttpResponse::METHOD_NOT_ALLOWED;
	if (Index::isIndexRoute(dataAdapter, actualRoute)) {
		return HttpResponse::DIR_LIST;
	}
	return HttpResponse::EMPTY;
}

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();

	if (connection->requestMode == Connection::SINGLE) {
		rtype = validateRoute(dataAdapter);
		if (rtype != HttpResponse::EMPTY) {
			response.setupResponse(rtype, dataAdapter);
			return;
		}
	}

	if (connection->isOverPayloadLimit) {
		connection->requestMode = Connection::SINGLE;
		response.setupResponse(HttpResponse::PAYLOAD_TOO_LARGE, dataAdapter);
		return ;
	}
	
	if (request.method == "GET") {
		if (request.isCgiRequest) {
			rtype = cgiAdapter.processCgi(dataAdapter);
			if (rtype != HttpResponse::EMPTY)
				response.setupResponse(rtype, dataAdapter);
			return;
		}
		else {
			rtype  = FileManager::readFile(dataAdapter);
			response.setupResponse(rtype, dataAdapter);
			connection->responseMode = (Connection::ResponseMode)response.isChunked();
		}
	}
	else if (request.method == "POST") {


		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.setupResponse(HttpResponse::CONTINUE, dataAdapter);
			return;
		}


		if (request.isCgiRequest) {
			
			for (std::vector<char>::iterator it = request.body.begin(); it != request.body.end(); ++it)
				cgiAdapter.body.push_back(*it);

			if (connection->contentLength == 0) {
				rtype = cgiAdapter.processCgi(dataAdapter);
				if (rtype != HttpResponse::EMPTY)
					response.setupResponse(rtype, dataAdapter);
			}
			return;
		}
		else {

			HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter);
			if (connection->contentLength == 0)
				response.setupResponse(rtype, dataAdapter);
			
		}
	}
	else if (request.method == "DELETE") {
		rtype = FileManager::deleteFile(dataAdapter);
		response.setupResponse(rtype, dataAdapter);
	}
	else {
		response.setupResponse(HttpResponse::METHOD_NOT_IMPLEMENTED, dataAdapter);
	}
	
}


