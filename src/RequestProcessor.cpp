#include <iostream>
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {

	HttpRequest& _request = dataAdapter.getRequest();
	HttpResponse& _response = dataAdapter.getResponse();

	_response.setVersion(HTTP_VERSION);
	if (_request.getMethod() == "GET") {
		FileManager::readFile(dataAdapter);
		if (_response.getStatusCode().empty()) {
			_response.setStatusCode("200");
			_response.setStatusMsg("OK");
			std::cout << BLUE << "Info: success 200 \"" << _request.getMethod() << "\", OK " << END << std::endl;
			std::cout << BLUE << "Connection fd: " << dataAdapter.getConnection()->getPollFd().fd << std::endl;
		}
		if (_response.getHeaders().find("Transfer-encoding")->second == "chunked")
			dataAdapter.getConnection()->isChunkedResponse = true;
	}
	else if (_request.getMethod() == "POST") {
		//TODO POST METHOD
	}
	else if (_request.getMethod() == "DELETE") {
		//TODO DELETE METHOD
		_response.setStatusCode("204");
		_response.setStatusMsg("NO_CONTENT");
		std::cout << BLUE << "Info: success 204 \"" << _request.getMethod() << "\", NO_CONTENT " << END << std::endl;
	}
	else {

 		_request.setUrl("/default/501.html"); //TODO hardcoded, debe obtener la ruta del config.

		FileManager::readFile(dataAdapter);
		_response.setStatusCode("501");
		_response.setStatusMsg("METHOD_NOT_IMPLEMENTED");
		std::cerr << YELLOW << "Warning: Error 501 \"" << _request.getMethod() << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl;
	}
}


