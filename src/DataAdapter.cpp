#include "DataAdapter.hpp"
#include "ConnectionManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "RequestProcessor.hpp"
#include "HttpHeader.hpp"

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


static void	parseRequestLine(std::stringstream& data, HttpRequest& request) {
	std::string	requestLine;

	std::getline(data, requestLine);
	request.setMethod(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	request.setUrl(requestLine.substr(0, requestLine.find(' ')));
	requestLine =  requestLine.substr(requestLine.find(' ') + 1, requestLine.size());
	request.setVersion(requestLine.substr(0, requestLine.find('\r')));
}


static void	parseHeaders(std::stringstream& data, HttpRequest& request) {
	std::string	headerLine;

	while (std::getline(data, headerLine)) {
		if (headerLine == "\r")
			break ;
		HttpHeader::addEntry(request, headerLine);
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
	std::stringstream	buffer;
	HeaderList			headers = _response.getHeaders();
	std::multimap<std::string, std::string>::iterator	it;

	buffer << _response.getVersion() << " " << _response.getStatusCode() << " " << _response.getStatusMsg() << CRLF;
	HttpHeader::appendHeaders(buffer, _response);
	buffer << CRLF;
	buffer << _response.getBody();
	_connection->sendBuffer = buffer.str();
}

Connection	*DataAdapter::getConnection() const { return _connection; }
HttpRequest& DataAdapter::getRequest() { return _request; }
HttpResponse& DataAdapter::getResponse() { return _response; }