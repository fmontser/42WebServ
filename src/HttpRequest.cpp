#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}
HttpRequest::~HttpRequest() {}
HttpRequest::HttpRequest(const HttpRequest& src) {
	_method = src._method;
	_url = src._url;
	_version = src._version;
	_headers = src._headers;
	_body = src._body;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& src) {
	if (this != &src) {
		_method = src._method;
		_url = src._url;
		_version = src._version;
		_headers = src._headers;
		_body = src._body;
	}
	return *this;
}

void	HttpRequest::clear() {
	_method.clear();
	_url.clear();
	_version.clear();
	_headers.clear();
	_body.clear();
}

std::string								HttpRequest::getMethod() const { return _method; }
std::string								HttpRequest::getUrl() const { return _url;}
std::string								HttpRequest::getVersion() const { return _version; }
HeaderList&								HttpRequest::getHeaders() { return _headers; }
std::string								HttpRequest::getBody() const { return _body; }

void	HttpRequest::setMethod(const std::string& method) { _method = method; }
void	HttpRequest::setUrl(const std::string& url) { _url = url; }
void	HttpRequest::setVersion(const std::string& version) { _version =  version; }
void	HttpRequest::addHeader(HeaderEntry header) { _headers.push_back(header); }
void	HttpRequest::setBody(const std::string& body) { _body =  body; }