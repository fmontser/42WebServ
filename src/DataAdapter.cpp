#include <sstream>
#include <algorithm>
#include <cstdio>
#include <algorithm>
#include "DataAdapter.hpp"
#include "ConnectionManager.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "HttpHeader.hpp"
#include "Utils.hpp"
#include "HttpProcessor.hpp"

#define SPLIT_CHR_SZ 1

DataAdapter::DataAdapter(Connection * connection) : _connection(connection) {
	isHeadersComplete = false;
	allowFileAppend = false;
}

DataAdapter::~DataAdapter() {}

DataAdapter::DataAdapter(const DataAdapter& src) {
	_request = src._request;
	_response = src._response;
	_connection = src._connection;
	isHeadersComplete = src.isHeadersComplete;
	allowFileAppend = src.allowFileAppend;
}

DataAdapter& DataAdapter::operator=(const DataAdapter& src) {
	if (this != &src) {
		_request = src._request;
		_response = src._response;
		_connection = src._connection;
		isHeadersComplete =src.isHeadersComplete;
		allowFileAppend = src.allowFileAppend;
	}
	return *this;
}

static void	deserializeRequestLine(std::stringstream& data, HttpRequest& request) {
	std::string	line;

	std::getline(data, line);
	request.method = line.substr(0, line.find(' '));
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.url = line.substr(0, line.find(' '));
	if (!request.url.empty() && request.url.at(request.url.size() -1) == '/')
		request.url.erase(request.url.size() - 1, 1);
	if (request.url.empty())
		request.url = "/";
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.version = line.substr(0, line.find('\r'));
}

static bool	 deserializeHeaders(std::stringstream& data, HttpRequest& request, DataAdapter& dataAdapter) {
	std::string	line;
	Connection	*connection = dataAdapter.getConnection();

	if (connection->requestMode == Connection::CHUNKS)
		return true;

	while (std::getline(data, line)) {
		line.append("\n");
		if (line == CRLF)
				break ;
		if (line != connection->boundStart) {
			request.addHeader(DataAdapter::deserializeHeader(line));
		}
	}

	if (connection->requestMode == Connection::PARTS)
		return true;
	return false;
}

static void removeBoundarie(std::vector<char>& body, const std::string& boundarie) {
	size_t boundary_len = boundarie.length();

	if (boundary_len == 0 || body.empty())
		return;
	std::vector<char>::iterator it = std::search(body.begin(), body.end(), boundarie.begin(), boundarie.end());
	if (it != body.end())
		body.erase(it, it + boundary_len);
}

static void	deserializeBody(std::stringstream& data, HttpRequest& request, DataAdapter& dataAdapter) {
	char	c[1];
	Connection *connection = dataAdapter.getConnection();
	size_t		bodySize = 0;

	while(data.get(*c)) {
		request.body.push_back(*c);
		bodySize++;
		if (bodySize > connection->getServer().getMaxPayload())
			connection->isOverPayloadLimit = true;
	}

	if (connection->requestMode == Connection::PARTS) {
		removeBoundarie(request.body, connection->boundStart);
		removeBoundarie(request.body, connection->boundEnd);
	}
}

HttpHeader	DataAdapter::deserializeHeader(std::string data) {
	HttpHeader newHeader;

	if (data.find(CRLF, 0) != data.npos)
		data.erase(data.find(CRLF, 0), CRLF_OFFSET);
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






#include "Config.hpp"
#include "Server.hpp"

static void	assingServer(DataAdapter& dataAdapter) {
	HttpRequest&					request = dataAdapter.getRequest();
	Connection						*actualConnection = dataAdapter.getConnection();
	std::string						hostName = request.getHostName();
	int 							port = actualConnection->getSocket().getPort();
	std::map<std::string, Server>&	serverList = Config::getServers();
	
	for (std::map<std::string, Server>::iterator server = serverList.begin(); server != serverList.end(); ++server) {

		std::vector<std::string>& serverHosts = server->second.getHosts();
		for (std::vector<std::string>::iterator host = serverHosts.begin(); host != serverHosts.end(); ++host) {
			if (*host == hostName) {
				actualConnection->setServer(server->second);
				return ;
			}
		}
	}

	for (std::map<std::string, Server>::iterator server = serverList.begin(); server != serverList.end(); ++server) {
		if (server->second.getPort() == port) {
			actualConnection->setServer(server->second);
			return ;
		}
	}
}




void	DataAdapter::deserializeRequest() {
	std::stringstream			data;

	data << std::string(_connection->recvBuffer.begin(), _connection->recvBuffer.end());

	if (_connection->requestMode == Connection::SINGLE)
		deserializeRequestLine(data, _request);

	if (!isHeadersComplete)
		isHeadersComplete = deserializeHeaders(data, _request, *this);

	if (!_connection->isOverPayloadLimit)
		deserializeBody(data, _request, *this);

	if (_connection->requestMode == Connection::CHUNKS && !getConnection()->hasChunksEnded)
		checkChunksEnd();

	if (!_connection->hasServerAssigned)
		assingServer(*this);
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
	int byte;
	std::stringstream	buffer;
	std::multimap<std::string, std::string>::iterator	it;

	if (_response.statusCode.empty())
		return;
	buffer << _response.version << " " << _response.statusCode << " " << _response.statusMsg << CRLF;
	serializeHeaders(buffer, _response);
	for (std::vector<char>::iterator it = _response.body.begin(); it != _response.body.end(); ++it)
		buffer << *it;
	while ((byte = buffer.get()) != EOF)
		_connection->sendBuffer.push_back(byte);
}


void DataAdapter::checkChunksEnd() {
	std::string						chuncksEnd = "0\r\n";
	std::vector<char>::iterator	it;

	if (!getConnection()->hasChunksEnded) {
		it = std::search(_request.body.begin(), _request.body.end(), chuncksEnd.begin(), chuncksEnd.end());
		if ( it != _request.body.end())
			getConnection()->hasChunksEnded = true;
	}
}

Connection		*DataAdapter::getConnection() const { return _connection; }
HttpRequest&	DataAdapter::getRequest() { return _request; }
HttpResponse&	DataAdapter::getResponse() { return _response; }