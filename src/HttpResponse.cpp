#include "HttpResponse.hpp"
#define CRLF "\r\n"

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}

void	HttpResponse::push() {
	std::stringstream response;
	std::multimap<std::string, std::string>::iterator it;

	response << version << " " << statusCode << " " << statusMsg << CRLF;
	for ( it = headers.begin(); it != headers.end(); ++it) {
		response << it->first << ": " << it->second << CRLF;
	}
	response << CRLF;
	response << body;
	

	//TODO enviar al socket
	std::string test = response.str();
}

void	HttpResponse::setVersion(std::string version) { this->version = version; }
void	HttpResponse::setStatusCode(std::string statusCode) { this->statusCode = statusCode; }
void	HttpResponse::setStatusMsg(std::string statusMsg) { this->statusMsg = statusMsg; }
void	HttpResponse::addHeader(std::string name, std::string value) {
	headers.insert(std::make_pair(name, value));
}
void	HttpResponse::setBody(std::string body) { this->body = body; }



