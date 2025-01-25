#include "DataAdapter.hpp"
#include "SocketManager.hpp"
#include "ServerConstants.hpp"

std::string	DataAdapter::_buffer;
HttpRequest	DataAdapter::_request;

DataAdapter::DataAdapter() {}
DataAdapter::~DataAdapter() {}

DataAdapter::DataAdapter(const DataAdapter& src) {
	_buffer = src._buffer;
	_request = src._request;
}

DataAdapter& DataAdapter::operator=(const DataAdapter& src) {
	if (this != &src) {
		_buffer = src._buffer;
		_request = src._request;
	}
	return *this;
}

static void	trimToken(std::string& str) {
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

void	DataAdapter::processRequest() {

}

void	DataAdapter::recieveData(std::string& request) {
	std::stringstream data;
	std::string requestLine, headerLine, headerKey, headerValue, bodyValue;
	std::pair<std::string, std::string> header;

	data << request;
	
	std::getline(data, requestLine);
	_request.setMethod(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_request.setUrl(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_request.setVersion(requestLine.substr(0, requestLine.find('\r')));

	while (std::getline(data, headerLine)) {
		if (headerLine == "\r")
			return ;
		headerKey = headerLine.substr(0, headerLine.find(':'));
		headerValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(headerKey);
		trimToken(headerValue);
		header = std::make_pair(headerKey, headerValue);
		_request.addHeader(header);
	}

	while (std::getline(data, bodyValue)) {
		bodyValue.append(bodyValue);
		bodyValue.append(std::string(1,'\n'));
	}
	_request.setBody(bodyValue);
	//processRequest(); //TODO recuperar

	//TEST FINAL RECICLAJE! HARCODED!

	HttpResponse test;

	test.setVersion(HTTP_VERSION);
	test.setStatusCode("200");
	test.setStatusMsg("OK");
	test.addHeader(std::make_pair("Content-length", "21"));
	test.setBody("<h1>Hello World!</h1>");
	sendData(test);
}

void	DataAdapter::sendData(HttpResponse& response) {
	std::stringstream	buffer;
	std::multimap<std::string, std::string>::iterator it;

	buffer << response.getVersion() << " " << response.getStatusCode() << " " << response.getStatusMsg() << CRLF;
	for ( it = _request.getHeaders().begin(); it != _request.getHeaders().end(); ++it) {
		buffer << it->first << ": " << it->second << CRLF;
	}
	buffer << CRLF;
	buffer << _request.getBody();

	SocketManager::sendResponse(buffer.str());
}