#include "HttpRequest.hpp"
#include "Utils.hpp"
#include "Connection.hpp"
#include <algorithm>

HttpRequest::HttpRequest() : HttpMessage() {}

HttpRequest::HttpRequest(const HttpRequest& src) : HttpMessage() {
	this->method = src.method;
	this->url = src.url;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& src) {
	if (this != &src) {
		this->method = src.method;
		this->url = src.url;
	}
	return *this;
}

HttpRequest::~HttpRequest() {}

bool	HttpRequest::handleMultipart(Connection *connection) {
	
	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		HttpHeader	header = *it;
		HeaderValue		value;
		HeaderProperty	property;

		if (header.getValue("Content-Type", &value) && value.name == "multipart/form-data") {
			if (value.getPropertie("boundary", &property)) {
				connection->requestMode = Connection::MULTIPART;
				connection->boundarie = property.value;
				connection->boundStart = "--";
				connection->boundStart.append(connection->boundarie);
				connection->boundStart.append("\r");
				connection->boundEnd =  connection->boundarie;
				connection->boundEnd.append("--\r");
			}
		}
		else if (header.getValue("Content-Length", &value))
			connection->contentLength = Utils::strToUint(value.name);
		if (connection->requestMode == Connection::MULTIPART && !connection->boundarie.empty() && connection->contentLength > 0)
			return true;
	}
	return false;
}