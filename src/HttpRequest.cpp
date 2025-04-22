#include "HttpRequest.hpp"
#include "Utils.hpp"
#include "Connection.hpp"
#include <algorithm>

HttpRequest::HttpRequest() : HttpMessage() {
	this->isBinaryDownload =false;
	this->isCgiRequest = false;
}

HttpRequest::HttpRequest(const HttpRequest& src) : HttpMessage() {
	this->method = src.method;
	this->url = src.url;
	this->isBinaryDownload = src.isBinaryDownload;
	this->isCgiRequest = src.isCgiRequest;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& src) {
	if (this != &src) {
		this->method = src.method;
		this->url = src.url;
		this->isBinaryDownload = src.isBinaryDownload;
		this->isCgiRequest = src.isCgiRequest;
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
				connection->requestMode = Connection::PARTS;
				connection->boundarie = property.value;
				connection->boundStart = "--";
				connection->boundStart.append(connection->boundarie);
				connection->boundEnd.append(CRLF);
				connection->boundEnd.append(connection->boundStart);
				connection->boundStart.append("\r\n");
				connection->boundEnd.append("--\r\n");
			}
		}
		else if (header.getValue("Content-Length", &value))
			connection->contentLength = Utils::strToUint(value.name);
		if (connection->requestMode == Connection::PARTS && !connection->boundarie.empty() && connection->contentLength > 0)
		return true;
	}
	return false;
}


bool	HttpRequest::handleChunked(Connection *connection) {
		connection->requestMode = Connection::CHUNKS;
		return true;
}

bool	HttpRequest::handlePostMode(Connection *connection) {
	HttpHeader		*header;
	HeaderValue		value;

	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		header = &(*it);
		if (header->getValue("Transfer-Encoding", &value) && value.name == "chunked")
			return handleChunked(connection);
	}

	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		header = &(*it);

		if (header->getValue("Content-Type", &value) && value.name == "multipart/form-data")
			return handleMultipart(connection);
		else if (header->getValue("Content-Type", &value) && value.name != "multipart/form-data")
			return false;
	}
	return false;
}

std::string HttpRequest::getCleanHost() const {
	for (std::vector<HttpHeader>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		if (Utils::toLower(it->name) == "host" && !it->values.empty()) {
			std::string host = it->values[0].name;
			size_t port_pos = host.find(':');
				return (port_pos != std::string::npos) ? host.substr(0, port_pos) : host;
				
			}
		}
		return "";
}