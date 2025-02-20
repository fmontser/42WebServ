#include "DataAdapter.hpp"
#include "ConnectionManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "RequestProcessor.hpp"


DataAdapter::DataAdapter(Connection * connection) : _connection(connection) {}
DataAdapter::~DataAdapter() {}

DataAdapter::DataAdapter(const DataAdapter& src) {
	_request = src._request;
	_response = src._response;
}

DataAdapter& DataAdapter::operator=(const DataAdapter& src) {
	if (this != &src) {
		_request = src._request;
		_response = src._response;
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

static void	parseRequestLine(std::stringstream& data, HttpRequest& request) {
	std::string	requestLine;

	std::getline(data, requestLine);
	request.setMethod(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	request.setUrl(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	request.setVersion(requestLine.substr(0, requestLine.find('\r')));
}

//TODO full header parser
static void	parseHeaders(std::stringstream& data, HttpRequest& request) {
	std::string	headerLine, headerKey, headerValue;
	std::pair<std::string, std::string> header;

	while (std::getline(data, headerLine)) {
		if (headerLine == "\r")
			break ;
		headerKey = headerLine.substr(0, headerLine.find(':'));
		headerValue = headerLine.substr(headerLine.find(':') + 1 ,headerLine.size());
		trimToken(headerKey);
		trimToken(headerValue);
		//TODO @@@@@ parsear headers completamente y estructurarlo (nuevas funciones)
		header = std::make_pair(headerKey, headerValue);
		request.addHeader(header);
	}
}

static void	parseBody(std::stringstream& data, HttpRequest& request) {
	std::string	bodyValue;

	while (std::getline(data, bodyValue)) {
		bodyValue.append(bodyValue);
		bodyValue.append(std::string(1,'\n'));
	}
	request.setBody(bodyValue);
}

void	DataAdapter::processData() {
	std::stringstream data;

	data << _connection->recvBuffer;
	parseRequestLine(data, _request);
	parseHeaders(data, _request);
	parseBody(data, _request);
	HttpProcessor::processHttpRequest(*this);
	processResponse();
	_request.clear();
	_response.clear();
}

void	DataAdapter::processResponse() {
	std::stringstream									buffer;
	std::multimap<std::string, std::string>				headers = _response.getHeaders();
	std::multimap<std::string, std::string>::iterator	it;

	buffer << _response.getVersion() << " " << _response.getStatusCode() << " " << _response.getStatusMsg() << CRLF;
	for ( it = headers.begin(); it != headers.end(); ++it) {
		buffer << it->first << ": " << it->second << CRLF;
	}
	buffer << CRLF;
	buffer << _response.getBody();
	_connection->sendBuffer = buffer.str();
}

Connection	*DataAdapter::getConnection() const { return _connection; }
HttpRequest& DataAdapter::getRequest() { return _request; }
HttpResponse& DataAdapter::getResponse() { return _response; }


//TODO header full parsing
/* std::string findSpecialChars(const std::string &str) {
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
} */