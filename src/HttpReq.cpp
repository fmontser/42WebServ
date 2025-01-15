#include "HttpReq.hpp"

HttpReq::HttpReq(std::stringstream *request) : request(request){
	parseRequestLine();
	parseHeaderFields();
	parseBody();
}

HttpReq::~HttpReq() {}

void	HttpReq::parseRequestLine() {
	std::string requestLine;
	
	std::getline(*request, requestLine);
	method = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	url = requestLine.substr(0, requestLine.find(' '));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	version = requestLine.substr(0, requestLine.find('\r'));
}

void	HttpReq::parseHeaderFields() {
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

void	HttpReq::parseBody() {
	std::string	bodyLine;

	while (std::getline(*request, bodyLine)) {
		body.append(bodyLine);
		body.append(std::string(1,'\n'));
	}
	body.at(body.size() - 1) = '\0';
}

void	HttpReq::trimToken(std::string& str) {
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