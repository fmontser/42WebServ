#include "HttpMessage.hpp"
#include "DataAdapter.hpp"

HttpMessage::HttpMessage() {}

HttpMessage::HttpMessage(const HttpMessage& src) {
	this->version = src.version;
	this->headers = src.headers;
	this->body = src.body;
}

HttpMessage& HttpMessage::operator=(const HttpMessage& src) {
	if (this != &src){
		this->version = src.version;
		this->headers = src.headers;
		this->body = src.body;
	}
	return *this;
}

HttpMessage::~HttpMessage() {}

void	HttpMessage::addHeader(HttpHeader header) {
	headers.push_back(header);
}

void	HttpMessage::addHeader(std::string header) {
	headers.push_back(DataAdapter::deserializeHeader(header));
}
