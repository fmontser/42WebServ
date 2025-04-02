#include "HttpResponse.hpp"
#include "ServerConstants.hpp"
#include <iostream>

HttpResponse::HttpResponse() : HttpMessage() {}

HttpResponse::HttpResponse(const HttpResponse& src) : HttpMessage() {
	this->statusCode = src.statusCode;
	this->statusMsg = src.statusMsg;
}
HttpResponse& HttpResponse::operator=(const HttpResponse& src) {
	if (this != &src) {
		this->statusCode = src.statusCode;
		this->statusMsg = src.statusMsg;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

bool	HttpResponse::isChunked() {
	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		HttpHeader header = *it;
		HeaderValue value;

		if (header.getValue("Transfer-Encoding", &value) && value.name == "chunked")
			return true;
	}
	return false;
}

void	HttpResponse::setupResponse(enum responseType responseType) {
	version = HTTP_VERSION;

	//TODO test borrrar
	std::string cbody("<!DOCTYPE html><html><body><h1>DONE!!</h1></body></html>");

	switch (responseType) {
		case CONTINUE:
			statusCode = "100";
			statusMsg = "CONTINUE";
			addHeader("Content-Length: 0");
			break;
		case OK:
			statusCode = "200";
			statusMsg = "OK";
			break;
		case CREATED:
			statusCode = "201";
			statusMsg = "CREATED";
			addHeader("Content-Length: 0");
 			body.assign(cbody.begin(), cbody.end()); //TODO asignar el body el default correspondiente.
			break;
		case NO_CONTENT:
			statusCode = "204";
			statusMsg = "NO_CONTENT";
			addHeader("Content-Length: 0");
			break;
		case SEE_OTHER:
 			statusCode = "303";
			statusMsg = "See Other";
			addHeader("Location: /");	//TODO obtener location de server config...?
 			addHeader("Content-Length: 0");
		case FORBIDDEN:
			statusCode = "403";
			statusMsg = "FORBIDDEN";
			addHeader("Content-Length: 0");
			break;
		case NOT_FOUND:
			statusCode = "404";
			statusMsg = "NOT_FOUND";
			addHeader("Content-Length: 0");
			break;
		case CONFLICT:
			statusCode = "409";
			statusMsg = "CONFLICT";
			addHeader("Content-Length: 0");
			break;
		case SERVER_ERROR:
			statusCode = "500";
			statusMsg = "SERVER_ERROR";
			addHeader("Content-Length: 0");
			break;
		case METHOD_NOT_IMPLEMENTED:
			statusCode = "501";
			statusMsg = "METHOD_NOT_IMPLEMENTED";
			addHeader("Content-Length: 0");
			break;
		default:
			break;
	}
	std::cout << BLUE << "Info: " << statusCode << ", " << statusMsg << END << std::endl;
}