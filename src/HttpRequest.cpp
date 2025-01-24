#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}
HttpRequest::~HttpRequest() {}
HttpRequest::HttpRequest(const HttpRequest& src) {
	_method = src._method;
	_url = src._url;
	_version = src._version;
	_headers = src._headers;
	_body = src._body;
	_data = src._data;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& src) {
	if (this != &src) {
		_method = src._method;
		_url = src._url;
		_version = src._version;
		_headers = src._headers;
		_body = src._body;
		_data = src._data;
	}
	return *this;
}

std::string								HttpRequest::getMethod() const { return _method; }
std::string								HttpRequest::getUrl() const { return _url;}
std::string								HttpRequest::getVersion() const { return _version; }
std::multimap<std::string, std::string>	HttpRequest::getHeaders() const { return _headers; }
std::string								HttpRequest::getBody() const { return _body; }

void	HttpRequest::setMethod(const std::string& method) { _method = method; }
void	HttpRequest::setUrl(const std::string& url) { _url = url; }
void	HttpRequest::setVersion(const std::string& version) { _version =  version; }
void	HttpRequest::addHeader(std::pair<std::string, std::string>& header) { _headers.insert(header); }
void	HttpRequest::setBody(const std::string& body) { _body =  body; }