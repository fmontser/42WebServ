#include "HttpResponse.hpp"
#define CRLF "\r\n"

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}

void	HttpResponse::push() {
	std::stringstream response;
	std::multimap<std::string, std::string>::iterator it;

	response << _version << " " << _statusCode << " " << _statusMsg << CRLF;
	for ( it = _headers.begin(); it != _headers.end(); ++it) {
		response << it->first << ": " << it->second << CRLF;
	}
	response << CRLF;
	response << _body;
	

	//TODO enviar al socket
	std::string test = response.str();
}

void	HttpResponse::setVersion(std::string version) { this->_version = version; }
void	HttpResponse::setStatusCode(std::string statusCode) { this->_statusCode = statusCode; }
void	HttpResponse::setStatusMsg(std::string statusMsg) { this->_statusMsg = statusMsg; }
void	HttpResponse::addHeader(std::string name, std::string value) {
	_headers.insert(std::make_pair(name, value));
}
void	HttpResponse::setBody(std::string body) { this->_body = body; }



