#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::~HttpRequest() {}

void	HttpRequest::pull(std::stringstream *request) {
	this->request = request;
	parseRequestLine();
	parseHeaderFields();
	parseBody();
}

void	HttpRequest::parseRequestLine() {
	std::string requestLine;
	
	std::getline(*request, requestLine);
	method = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	url = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	version = requestLine.substr(0, requestLine.find('\r'));
}

void	HttpRequest::parseHeaderFields() {
	std::string	headerLine;
	std::string	fieldName;
	std::string	fieldValue;

	while (std::getline(*request, headerLine)) {
		if (headerLine == "\r")
			return ;
		fieldName = headerLine.substr(0, headerLine.find(':'));
		fieldValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(fieldName);
		trimToken(fieldValue);
		headers.insert(std::make_pair(fieldName, fieldValue));
	}
}

void	HttpRequest::parseBody() {
	std::string	bodyLine;

	while (std::getline(*request, bodyLine)) {
		body.append(bodyLine);
		body.append(std::string(1,'\n'));
	}
	body.at(body.size() - 1) = '\0';
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


std::string								HttpRequest::getMethod() { return method; }
std::string								HttpRequest::getUrl() { return url;}
std::string								HttpRequest::getVersion() { return version; }
std::multimap<std::string, std::string>	HttpRequest::getHeaders() { return headers; }
std::string								HttpRequest::getBody() { return body; }