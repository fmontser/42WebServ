#include "HttpResponse.hpp"
#include <sys/socket.h>

#define CRLF "\r\n"

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}

void	HttpResponse::push() {
	std::stringstream	buffer;
	std::string			msg;
	std::multimap<std::string, std::string>::iterator it;

	buffer << _version << " " << _statusCode << " " << _statusMsg << CRLF;
	for ( it = _headers.begin(); it != _headers.end(); ++it) {
		buffer << it->first << ": " << it->second << CRLF;
	}
	buffer << CRLF;
	buffer << _body;

	msg = buffer.str(); 
	send(4, msg.c_str(), msg.length(), 0); //TODO quitar el puerto hardcoded
}

void	HttpResponse::setVersion(std::string version) { this->_version = version; }
void	HttpResponse::setStatusCode(std::string statusCode) { this->_statusCode = statusCode; }
void	HttpResponse::setStatusMsg(std::string statusMsg) { this->_statusMsg = statusMsg; }
void	HttpResponse::addHeader(std::string name, std::string value) {
	_headers.insert(std::make_pair(name, value));
}
void	HttpResponse::setBody(std::string body) { this->_body = body; }