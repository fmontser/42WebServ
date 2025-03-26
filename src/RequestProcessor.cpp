#include <iostream>
#include "HttpHeader.hpp"
#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"
#include "FileManager.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {

	HttpRequest&	request = dataAdapter.getRequest();
	HttpResponse&	response = dataAdapter.getResponse();
	Connection		*connection = dataAdapter.getConnection();

	response.version = HTTP_VERSION;
	if (request.method == "GET") {
		FileManager::readFile(dataAdapter);
		if (response.statusCode.empty()) {
			response.statusCode = "200";
			response.statusMsg = "OK";
			std::cout << BLUE << "Info: success 200 \"" << request.method << "\", OK " << END << std::endl;
			std::cout << BLUE << "Connection fd: " << connection->getPollFd().fd << std::endl;
		}
		connection->isChunkedResponse = response.isChunked();
	}
	else if (request.method == "POST") {
		if (connection->requestMode == Connection::SINGLE && request.handleMultipart(connection)) {
			response.version = HTTP_VERSION;
			response.statusCode = "100";
			response.statusMsg = "CONTINUE";
			return ;
		}
		FileManager::writeFile(dataAdapter); //TODO write unimplemented
		if (response.statusCode.empty() && dataAdapter.getConnection()->contentLength == 0) {
			response.statusCode = "201";
			response.statusMsg = "CREATED";
			std::cout << BLUE << "Info: success 201 \"" << request.method << "\", CREATED " << END << std::endl;
			std::cout << BLUE << "Connection fd: " << connection->getPollFd().fd << std::endl;
		}
		else {
			response.version = HTTP_VERSION;
			response.statusCode = "100";
			response.statusMsg = "CONTINUE";
		}
	}
	else if (request.method == "DELETE") {
		//TODO DELETE METHOD
		response.statusCode = "204";
		response.statusMsg = "NO_CONTENT";
		std::cout << BLUE << "Info: success 204 \"" << request.method << "\", NO_CONTENT " << END << std::endl;
	}
	else {

 		request.url = "/default/501.html"; //TODO hardcoded, debe obtener la ruta del config.

		FileManager::readFile(dataAdapter);
		response.statusCode = "501";
		response.statusMsg = "METHOD_NOT_IMPLEMENTED";
		std::cerr << YELLOW << "Warning: Error 501 \"" << request.method << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl;
	}
}


