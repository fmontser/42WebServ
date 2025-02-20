#include "HttpResponse.hpp"

HttpResponse::HttpResponse() {}
HttpResponse::~HttpResponse() {}
HttpResponse::HttpResponse(const HttpResponse& src) {
	_version = src._version;
	_statusCode = src._statusCode;
	_statusMsg = src._statusMsg;
	_headers = src._headers;
	_body = src._body;
}
HttpResponse& HttpResponse::operator=(const HttpResponse& src) {
	if (this != &src) {
		_version = src._version;
		_statusCode = src._statusCode;
		_statusMsg = src._statusMsg;
		_headers = src._headers;
		_body = src._body;
	}
	return *this;
}

void	HttpResponse::clear() {
	_version.clear();
	_statusCode.clear();
	_statusMsg.clear();
	_headers.clear();
	_body.clear();
}


void		HttpResponse::setVersion(std::string version) { this->_version = version; }
void		HttpResponse::setStatusCode(std::string statusCode) { this->_statusCode = statusCode; }
void		HttpResponse::setStatusMsg(std::string statusMsg) { this->_statusMsg = statusMsg; }
HeaderList&	HttpResponse::getHeaders() { return _headers; }
void		HttpResponse::setBody(std::string body) { this->_body = body; }

std::string								HttpResponse::getVersion() const { return _version; }
std::string								HttpResponse::getStatusCode() const { return _statusCode; }
std::string								HttpResponse::getStatusMsg() const { return _statusMsg; }
void									HttpResponse::addHeader(HeaderEntry header) { _headers.push_back(header); }
std::string								HttpResponse::getBody() const { return _body; }