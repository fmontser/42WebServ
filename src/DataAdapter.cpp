#include "DataAdapter.hpp"
#include "SocketManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"

struct ParsedHeader {
	std::string mainValue;
	std::map<std::string, std::string> params;
};


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

std::string findSpecialChars(const std::string &str) {
	const std::string specialChars = ",;=/-\"@<>[]{}& \t+.-_";
	size_t pos = str.find_first_of(specialChars);
	if (pos != std::string::npos)
		return str.substr(0, pos);
	return str;
}

ParsedHeader parsedHeaderValue(const std::string &headerValue){
	ParsedHeader result;

	size_t pos = headerValue.find(';');
	if (pos != std::string::npos) {
		result.mainValue = findSpecialChars(headerValue.substr(0, pos));
	} else {
		result.mainValue = findSpecialChars(headerValue);
	}
	if (pos != std::string::npos) {
		std::string params = headerValue.substr(pos + 1);
		std::stringstream paramsStream(params);
		std::string param;

		while (std::getline(paramsStream, param, ';')) {
			size_t eqPos = param.find('=');
			if (eqPos != std::string::npos) {
				std::string key = param.substr(0, eqPos);
				std::string value = param.substr(eqPos + 1);
				trimToken(key);
				trimToken(value);
				result.params[key] = value;
			}
		}
	}
	return result;
}

void	DataAdapter::recieveData(Socket *targetSocket, std::string& request) {
	std::stringstream data;
	std::string requestLine, headerLine, headerKey, headerValue, bodyValue, processedHeaderValues;
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
			break ;
		headerKey = headerLine.substr(0, headerLine.find(':'));
		headerValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(headerKey);
		trimToken(headerValue);
		ParsedHeader parsedHeader = parsedHeaderValue(headerValue);
		header = std::make_pair(headerKey, headerValue);
		_request.addHeader(header);

		for (std::map<std::string, std::string>::iterator it = parsedHeader.params.begin(); it != parsedHeader.params.end(); ++it) {
			std::string key = headerKey + "-" + it->first;
			header = std::make_pair(key, it->second);
			_request.addHeader(header);
		}
	}

	while (std::getline(data, bodyValue)) {
		bodyValue.append(bodyValue);
		bodyValue.append(std::string(1,'\n'));
	}
	_request.setBody(bodyValue);
	FileManager::recieveHttpRequest(targetSocket, _request);
	_request.clear();
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