#include "HttpResponse.hpp"

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}

void	HttpResponse::push() {
	std::string response;

	//TODO @@@@@ construir la respuesta...
	//response.append

	//TODO enviar al socket
}

void	HttpResponse::setVersion(std::string version) { this->version = version; }
void	HttpResponse::setStatusCode(std::string statusCode) { this->statusCode = statusCode; }
void	HttpResponse::setStatusMsg(std::string statusMsg) { this->statusMsg = statusMsg; }
void	HttpResponse::addHeader(std::string name, std::string value) {
	headers.insert(std::make_pair(name, value));
}
void	HttpResponse::setBody(std::string body) { this->body = body; }


