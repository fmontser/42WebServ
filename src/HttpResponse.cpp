#include "HttpResponse.hpp"

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