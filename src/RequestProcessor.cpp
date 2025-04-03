#include <iostream>
#include "HttpHeader.hpp"
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	
	HttpResponse::responseType rtype;

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();
	if (request.method == "GET") {
		rtype  = FileManager::readFile(dataAdapter);
		if (rtype == HttpResponse::NOT_FOUND || rtype == HttpResponse::FORBIDDEN ){	//TODO FORBIDDEN
			dataAdapter.getRequest().url = "/default/404.html"; //TODO hardcoded obtener de server
			response.setupResponse(FileManager::readFile(dataAdapter));
			return;
		}
		else if (rtype != HttpResponse::OK || (response.statusCode.empty()))
			response.setupResponse(rtype);
		connection->isChunkedResponse = response.isChunked();
	}
	else if (request.method == "POST") {
		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.setupResponse(HttpResponse::CONTINUE);
			return;
		}
		HttpResponse::responseType rtype = FileManager::writeFile(dataAdapter);
		if (connection->contentLength == 0) {
			response.setupResponse(rtype);
			if (rtype == HttpResponse::CREATED){
				dataAdapter.getRequest().url = "/defaults/201.html";
				FileManager::readFile(dataAdapter);
			}
		}
	}
	else if (request.method == "DELETE") {
		response.setupResponse(FileManager::deleteFile(dataAdapter));
	}
	else {
		request.url = "/default/501.html"; //TODO hardcoded, debe obtener la ruta del config.
		FileManager::readFile(dataAdapter);
		response.setupResponse(HttpResponse::METHOD_NOT_IMPLEMENTED);
	}
	
}


