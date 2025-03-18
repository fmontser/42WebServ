#include <sstream>
#include "DataAdapter.hpp"
#include "ConnectionManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "HttpHeader.hpp"
#include "Utils.hpp"

#define SPLIT_CHR_SZ 1

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

static void	deserializeRequestLine(std::stringstream& data, HttpRequest& request) {
	std::string	line;

	std::getline(data, line);
	request.method = line.substr(0, line.find(' '));
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.url = line.substr(0, line.find(' '));
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.version = line.substr(0, line.find('\r'));
}



static void	deserializeHeaders(std::stringstream& data, HttpRequest& request, Connection *connection) {
	std::string	line;

	while (std::getline(data, line)) {
		if (line == "\r")
				break ;
		if (line != connection->boundStart)
			request.addHeader(DataAdapter::deserializeHeader(line));
	}
}

static void	deserializeBody(std::stringstream& data, HttpRequest& request, Connection *connection) {
	std::string	line;

	//TODO limpiar e usar connection->bounds (cuando ya funcione...)

	std::string boundarieLine = "--";
	std::string endboundarieLine;

	if (connection->isMultipartUpload) {

		boundarieLine.append(connection->boundarie);
		endboundarieLine.append (boundarieLine);
		endboundarieLine.append("--\r");
		boundarieLine.append("\r");

		while (std::getline(data, line)) {
			if (line != boundarieLine) {
				if ( line == endboundarieLine)
					break;
				request.body.append(line);
				request.body.append(std::string(1,'\n'));
			}
		}
	}
	else {
		while (std::getline(data, line)) {
				request.body.append(line);
				request.body.append(std::string(1,'\n'));
		}
	}
}

HttpHeader	DataAdapter::deserializeHeader(std::string data) {
	HttpHeader newHeader;
	newHeader.name = data.substr(0, data.find(':'));
	data = data.substr(data.find(':') + SPLIT_CHR_SZ, data.size());
	Utils::trimString(data);
	std::vector<std::string> values = Utils::splitString(data, ',');
	for (std::vector<std::string>::iterator valueIt = values.begin(); valueIt != values.end(); ++valueIt) {
		std::string val = *valueIt;
		Utils::trimString(val);
		HeaderValue newValue;
		std::vector<std::string> properties = Utils::splitString(val, ';');
		for (std::vector<std::string>::iterator propertyIt = properties.begin(); propertyIt != properties.end(); ++propertyIt) {
			if (propertyIt == properties.begin())
				newValue.name = *propertyIt;
			else {
				std::string prop = *propertyIt;
				Utils::trimString(prop);
				HeaderProperty newProperty;
				std::vector<std::string> properties = Utils::splitString(prop, '=');
				newProperty.name = properties.front();
				if (properties.size() == 2)
					newProperty.value = properties.back();
				newValue.addProperty(newProperty);
			}
		}
		newHeader.addValue(newValue);
	}
	return newHeader;
}


void	DataAdapter::deserializeRequest() {
	std::stringstream data;

	data << _connection->recvBuffer;

	if (!_connection->isMultipartUpload)
		deserializeRequestLine(data, _request);
	deserializeHeaders(data, _request, _connection);
	deserializeBody(data, _request, _connection);
}

static void	serializeHeaders(std::stringstream& buffer, HttpResponse& response) {
	for (std::vector<HttpHeader>::iterator headerIt = response.headers.begin(); headerIt != response.headers.end(); ++headerIt) {
		buffer << headerIt->name << ": ";
		for (std::vector<HeaderValue>::iterator valueIt = headerIt->values.begin(); valueIt != headerIt->values.end(); ++valueIt) {
			buffer << valueIt->name;
			if (valueIt->properties.size() > 0) {
				for (std::vector<HeaderProperty>::iterator properyIt = valueIt->properties.begin(); properyIt != valueIt->properties.end(); ++properyIt) {
					buffer << "; ";
					buffer << properyIt->name;
					if (properyIt->value != "")
						buffer << "=" << properyIt->value;
				}
			}
			if (valueIt + 1 != headerIt->values.end())
				buffer << ", ";
		}
		buffer << CRLF;
	}
	buffer << CRLF;
}

void	DataAdapter::serializeResponse() {
	std::stringstream	buffer;
	std::multimap<std::string, std::string>::iterator	it;

	buffer << _response.version << " " << _response.statusCode << " " << _response.statusMsg << CRLF;
	serializeHeaders(buffer, _response);
	buffer << _response.body;
	_connection->sendBuffer = buffer.str();
}

Connection		*DataAdapter::getConnection() const { return _connection; }
HttpRequest&	DataAdapter::getRequest() { return _request; }
HttpResponse&	DataAdapter::getResponse() { return _response; }