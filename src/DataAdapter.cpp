#include "DataAdapter.hpp"
#include "SocketManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include <cstdio>
#include <cstdlib>

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

void	DataAdapter::recieveData(Socket *targetSocket, std::string& request) {
	std::stringstream data;
	std::string requestLine, headerLine, headerKey, headerValue, bodyValue;
	std::pair<std::string, std::string> header;
	std::string contentType; //created a variable to store content type 

	data << request;
	
	std::getline(data, requestLine);
	_request.setMethod(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_request.setUrl(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	_request.setVersion(requestLine.substr(0, requestLine.find('\r')));

	while (std::getline(data, headerLine)) {
		if (headerLine == "\r")
			break ;
		headerKey = headerLine.substr(0, headerLine.find(':'));
		headerValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(headerKey);
		trimToken(headerValue);

		if (headerKey == "Content-Type") //created a condition to store content type
			contentType = headerValue;

		header = std::make_pair(headerKey, headerValue);
		_request.addHeader(header);
	}


	parseRequestBody(data, _request, contentType); //new function to parse body

	/* //THE FOLLOWING BLOCK IS MOVED TO NEW FUNCTTION parseRequestBody:
		while (std::getline(data, bodyValue)) {
		bodyValue.append(bodyValue);
		bodyValue.append(std::string(1,'\n'));
	}

	_request.setBody(bodyValue);
 */
	FileManager::recieveHttpRequest(targetSocket, _request);
	_request.clear();
}

std::string decodeURLEncoded(const std::string& str) { //new helper-function to decode url encoded strings
	std::string result;// std::string result created for storing the decoded string
	char hex[3] = {0};// char array hex created to store the hex value	

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '%') {//if the character is %, then the next two characters are hex values
			hex[0] = str[i + 1];//first hex value stored in hex[0], str[i] should be %, so i+1 is the first hex value 
			hex[1] = str[i + 2];//second hex value stored in hex[1], str[i+1] should be the first hex value, so i+2 is the second hex value
			result += static_cast<char>(std::strtol(hex, NULL, 16));//hex values are converted to decimal and then to char and stored in result
			i += 2;//increment i by 2 to skip the hex values
		} else if (str[i] == '+') {//if the character is +, then it is a space
			result += ' ';
		} else {
			result += str[i];//if the character is not % or +, then it is a normal character and stored in result
		}
	}
	return result;
} 

/* std::string DataAdapter::decodeURLEncoded(const std::string& str) { //new helper-function to decode url encoded strings
DOES NOT WORK AS A STATIC FUNCTION.. ??
	std::string result;
	char hex[3] = {0};

	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '%') {
			hex[0] = str[i + 1];
			hex[1] = str[i + 2];
			result += static_cast<char>(std::strtol(hex, NULL, 16));
			i += 2;
		} else if (str[i] == '+') {
			result += ' ';
		} else {
			result += str[i];
		}
	}
	return result;
} */

std::string DataAdapter::extractBoundary(const std::string& contentType) { //new helper-function to extract boundary from content-type
	std::size_t pos = contentType.find("boundary=");

	if (pos != std::string::npos)
		return contentType.substr(pos + 9);//as boundary= is 9 characters long, we start from pos+9
	return "";
}

std::string DataAdapter::parseMultipartData(const std::string& body, const std::string& boundary) { //new helper-function to parse multipart data
	std::string parsed;
	std::string line;
	std::stringstream data(body);
	std::string boundaryLine = "--" + boundary;//boundary line is --boundary; so we add -- to the boundary
	bool isBoundary = false;

	while (std::getline(data, line)) {
		if (line == boundaryLine) {
			isBoundary = true;
			continue;
		}
		if (isBoundary) {
			parsed.append(line);
			parsed.append(std::string(1,'\n'));
		}
	}
	return parsed;
}

void DataAdapter::parseRequestBody(std::stringstream& data, HttpRequest& request, const std::string& contentType) {
	std::string body;
	std::string line;

		while (std::getline(data, line)) {
		body.append(line);
		body.append(std::string(1,'\n'));
	}
	if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
		body = decodeURLEncoded(body);
	} else if (contentType.find("multipart/form-data") != std::string::npos) {
		std::string boundary = extractBoundary(contentType);
		body = parseMultipartData(body, boundary);
	} else if (contentType.find("application/json") != std::string::npos) {
		//parse json here.. 
	}

	request.setBody(body);
}

void	DataAdapter::sendData(Socket *targetSocket, HttpResponse& response) {
	std::stringstream									buffer;
	std::multimap<std::string, std::string>				headers = response.getHeaders();
	std::multimap<std::string, std::string>::iterator	it;
	bool												hasChunks = false;

	buffer << response.getVersion() << " " << response.getStatusCode() << " " << response.getStatusMsg() << CRLF;
	for ( it = headers.begin(); it != headers.end(); ++it) {
		buffer << it->first << ": " << it->second << CRLF;
	}
	buffer << CRLF;
	buffer << response.getBody();

	//TODO test chunks!
	it = response.getHeaders().find("Transfer-Encoding");
	if (it != response.getHeaders().end() && it->second == "chunked")
		hasChunks = true;
	SocketManager::recieveResponse(targetSocket, buffer.str(), hasChunks);
}