#include "HttpRequest.hpp"
#include "Utils.hpp"

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
				connection->isMultipartUpload = true;
				connection->boundarie = property.name;
				continue ;
			}
		}
		if (header.getValue("Content-Length", &value)) {
			connection->contentLength = Utils::strToUint(value.name);
		}
	}
	return false;
}