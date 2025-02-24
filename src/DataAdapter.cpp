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

static void	deSerializeRequestLine(std::stringstream& data, HttpRequest& request) {
	std::string	line;

	std::getline(data, line);
	request.method = line.substr(0, line.find(' '));
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.url = line.substr(0, line.find(' '));
	line =  line.substr(line.find(' ') + SPLIT_CHR_SZ, line.size());
	request.version = line.substr(0, line.find('\r'));
}

static void	deSerializeHeaders(std::stringstream& data, HttpRequest& request) {
	std::string	line;

	while (std::getline(data, line)) {
		
		if (line == "\r")
			break ;
		//Nombre de header
		HttpHeader newHeader(line.substr(0, line.find(':')));
		line = line.substr(line.find(':') + SPLIT_CHR_SZ, line.size());
		Utils::trimString(line);


		//Valores
		std::vector<std::string> values = Utils::splitString(line, ',');
		for (std::vector<std::string>::iterator vit = values.begin(); vit != values.end(); ++vit) {
			std::string v = *vit;
			Utils::trimString(v);


			std::vector<std::string> properties = Utils::splitString(v, ';');
			for (std::vector<std::string>::iterator pit = properties.begin(); pit != properties.end(); ++pit) {
				if (pit == properties.begin()) {

					//TODO @@@@@@@@ hayque sacar value de aqui para poder asignarle props!
					HeaderValue value(*pit);
					newHeader.addValue(value);
				}
				else {
					std::string p = *pit;
					Utils::trimString(p);

					std::vector<std::string> properties = Utils::splitString(p, '=');
					//TODO @@@@@@@@ asignar props a value!
					//value
				}


			}

		
		}

/* 		//Valores
		while (line.size()) {
			std::string value = line.substr(0, line.find(','));
			HeaderValue newValue(value);

			//Propiedades
			if (value.c {
				while (value.size()) {
					std::string propertie = line.substr(0, line.find(';'));
					HeaderProperty newPropertie(propertie.substr(0, propertie.find('='))
					, propertie.substr(propertie.find('=') + SPLIT_CHR_SZ, propertie.size()));
					newValue.addProperty(newPropertie);
					value = value.substr(propertie.size(), value.size());
				}
			}
			newHeader.addValue(newValue);
			line = line.substr(value.size(), line.size());
		} */

	}

	(void)request;
}

static void	deSerializeBody(std::stringstream& data, HttpRequest& request) {
	std::string	bodyValue;

	while (std::getline(data, bodyValue)) {
		bodyValue.append(bodyValue);
		bodyValue.append(std::string(1,'\n'));
	}
	request.body = bodyValue;
}

void	DataAdapter::deSerializeRequest() {
	std::stringstream data;

	data << _connection->recvBuffer;
	deSerializeRequestLine(data, _request);
	deSerializeHeaders(data, _request);
	deSerializeBody(data, _request);
}


static void	serializeHeaders(std::stringstream& buffer, HttpResponse& response) {
	(void)buffer;
	(void)response;
	//TODO
}


void	DataAdapter::serializeResponse() {
	std::stringstream	buffer;
	std::multimap<std::string, std::string>::iterator	it;

	buffer << _response.version << " " << _response.statusCode << " " << _response.statusMsg << CRLF;
	serializeHeaders(buffer, _response);
	buffer << CRLF;
	buffer << _response.body;
	_connection->sendBuffer = buffer.str();
}

Connection	*DataAdapter::getConnection() const { return _connection; }
HttpRequest& DataAdapter::getRequest() { return _request; }
HttpResponse& DataAdapter::getResponse() { return _response; }


/* */

//TODO @@@@@@@@@@@@@@@@@@@@@@00000000 ALGO NO COMPILA RELACIONADO CON STD::PAIR


//TODO churrificar
/*
void			HttpHeader::appendHeaders(std::stringstream& buffer, HttpResponse& response) {
	//HeaderList&			headers = response.getHeaders();
	(void)response;
	(void)buffer;

	//TODO @@@@@@@22222 necesita testing, no esta bien fijo!

 	for (HeaderList::iterator header = headers.begin(); header != headers.end(); ++header) {

		buffer << header->first << ": ";
		for (ValueMap::iterator value = header->second.begin(); value != header->second.end(); ++value) {
			buffer << value->first;
			if (value->second.empty())
				buffer << ", ";
			else {
				for (PropertyMap::iterator property = value->second.begin(); property != value->second.end(); ++property)
					buffer << "; " << property->first << "=" << property->second;
			}
		}
		buffer << CRLF;
	}
}
*/