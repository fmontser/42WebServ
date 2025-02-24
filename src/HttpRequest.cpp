#include "HttpRequest.hpp"

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