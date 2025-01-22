#include "HttpRequest.hpp"
#include <fstream>
#include <iostream>
#include <sstream> 


HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

void	HttpRequest::pull(std::stringstream *request, int requestSocket) {
	this->_request = request;
	_socket = requestSocket;
	parseRequestLine();
	parseHeaderFields();
	parseBody();
}

void	HttpRequest::parseRequestLine() {
	std::string requestLine;
	
	std::getline(*_request, requestLine);
	_method = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_url = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_version = requestLine.substr(0, requestLine.find('\r'));

	/* if (_url == "/favicon.ico") {
		if (_method != "GET") { //
			_headers.insert(std::make_pair("HTTP/1.1", "405 Method Not Allowed"));
			_body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
			return;
		}

		std::ifstream faviconFile("favicon.ico", std::ios::binary);
		if (!faviconFile.is_open()) {
			std::cerr << "Error: No se pudo abrir favicon.ico" << std::endl;
			_headers.insert(std::make_pair("HTTP/1.1", "404 Not Found"));
			_body = "<html><body><h1>404 Not Found</h1></body></html>";
			return;
		}

		std::ostringstream content;
		content << faviconFile.rdbuf();
		_body = content.str();
		_headers.insert(std::make_pair("HTTP/1.1", "200 OK"));
		_headers.insert(std::make_pair("Content-Type", "image/x-icon"));
		_headers.insert(std::make_pair("Content-Length", to_string(_body.size())));
		_headers.insert(std::make_pair("Connection", "close"));
		return;
	} */
}

void	HttpRequest::parseHeaderFields() {
	std::string	headerLine;
	std::string	fieldName;
	std::string	fieldValue;

	while (std::getline(*_request, headerLine)) {
		if (headerLine == "\r")
			return ;
		fieldName = headerLine.substr(0, headerLine.find(':'));
		fieldValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(fieldName);
		trimToken(fieldValue);
		_headers.insert(std::make_pair(fieldName, fieldValue));
	}
}

void	HttpRequest::parseBody() {
	std::string	bodyLine;

	while (std::getline(*_request, bodyLine)) {
		_body.append(bodyLine);
		_body.append(std::string(1,'\n'));
	}
	//TODO borrar _body.at(_body.size() - 1) = '\0';
}

void	HttpRequest::trimToken(std::string& str) {
	size_t start = 0;
	size_t end = str.length() - 1;

	while (start <= end && std::isspace(str[start])) {
		start++;
	}
	while (end >= start && (std::isspace(str[end]) || str[end] == '\r')) {
		end--;
	}
	str = str.substr(start, end - start + 1);
}


std::string								HttpRequest::getMethod() const { return _method; }
std::string								HttpRequest::getUrl() const { return _url;}
std::string								HttpRequest::getVersion() const { return _version; }
std::multimap<std::string, std::string>	HttpRequest::getHeaders() const { return _headers; }
std::string								HttpRequest::getBody() const { return _body; }
int										HttpRequest::getSocket() const { return _socket; }

/* std::string HttpRequest::to_string(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
 */