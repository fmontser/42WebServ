#include <iostream>
#include "HttpHeader.hpp"
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {

	HttpRequest& _request = dataAdapter.getRequest();
	HttpResponse& _response = dataAdapter.getResponse();

	_response.version = HTTP_VERSION;
	if (_request.method == "GET") {
		FileManager::readFile(dataAdapter);
		if (_response.statusCode.empty()) {
			_response.statusCode = "200";
			_response.statusMsg = "OK";
			std::cout << BLUE << "Info: success 200 \"" << _request.method << "\", OK " << END << std::endl;
			std::cout << BLUE << "Connection fd: " << dataAdapter.getConnection()->getPollFd().fd << std::endl;
		}
		//TODO arreglar acceso al header.
/* 		if (_response.getHeaders().find("Transfer-encoding")->second == "chunked")
			dataAdapter.getConnection()->isChunkedResponse = true; */
	}
	else if (_request.method == "POST") {
		//TODO POST METHOD
	}
	else if (_request.method == "DELETE") {
		//TODO DELETE METHOD
		_response.statusCode = "204";
		_response.statusMsg = "NO_CONTENT";
		std::cout << BLUE << "Info: success 204 \"" << _request.method << "\", NO_CONTENT " << END << std::endl;
	}
	else {

 		_request.url = "/default/501.html"; //TODO hardcoded, debe obtener la ruta del config.

		FileManager::readFile(dataAdapter);
		_response.statusCode = "501";
		_response.statusMsg = "METHOD_NOT_IMPLEMENTED";
		std::cerr << YELLOW << "Warning: Error 501 \"" << _request.method << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl;
	}
}


