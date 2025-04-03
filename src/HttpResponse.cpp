#include "HttpResponse.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include <iostream>
#include <sstream>

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
	std::string contentLength ("Content-Length: ");
	std::stringstream ss;

	ss << body.size();
	contentLength.append(ss.str());
	version = HTTP_VERSION;

	switch (responseType) {
		case CONTINUE:
			statusCode = "100";
			statusMsg = "CONTINUE";
			addHeader(contentLength);
			break;
		case OK:
			statusCode = "200";
			statusMsg = "OK";
			break;
		case CREATED:
			statusCode = "201";
			statusMsg = "CREATED";
			//TODO!!!
			//addHeader("Content-Length: 56");
 			//body.assign(cbody.begin(), cbody.end()); //TODO asignar el body el default correspondiente.
			break;
		case NO_CONTENT:
			statusCode = "204";
			statusMsg = "NO_CONTENT";
			break;
		case SEE_OTHER:
 			statusCode = "303";
			statusMsg = "See Other";
			addHeader("Location: /");	//TODO obtener location de server config...?
 			addHeader(contentLength);
			break;
		case FORBIDDEN:
			statusCode = "403";
			statusMsg = "FORBIDDEN";
			addHeader(contentLength);
			break;
		case NOT_FOUND:
			statusCode = "404";
			statusMsg = "NOT_FOUND";
			addHeader(contentLength);
			break;
		case CONFLICT:
			statusCode = "409";
			statusMsg = "CONFLICT";
			addHeader(contentLength);
			break;
		case SERVER_ERROR:
			statusCode = "500";
			statusMsg = "SERVER_ERROR";
			addHeader(contentLength);
			break;
		case METHOD_NOT_IMPLEMENTED:
			statusCode = "501";
			statusMsg = "METHOD_NOT_IMPLEMENTED";
			break;
		default:
			break;
	}
	std::cout << BLUE << "Info: " << statusCode << ", " << statusMsg << END << std::endl;
}