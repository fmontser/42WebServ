#include "HttpHeader.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "ServerConstants.hpp"

HttpHeader::HttpHeader() {}
HttpHeader::HttpHeader(const HttpHeader& src) { (void)src; }
HttpHeader& HttpHeader::operator=(const HttpHeader& src) { (void)src; }
HttpHeader::~HttpHeader() {}

/* static void	trimToken(std::string& str) {
	size_t start = 0;
	size_t end = str.length() - 1;

	while (start <= end && std::isspace(str[start])) {
		start++;
	}
	while (end >= start && (std::isspace(str[end]) || str[end] == '\r')) {
		end--;
	}
	str = str.substr(start, end - start + 1);
} */

//TODO @@@@@@@@@@@@@@@@@@@@@@00000000 ALGO NO COMPILA RELACIONADO CON STD::PAIR

static HeaderEntry parseHeader(std::string headerLine) {
	//TODO @@@@@@11111 implementar parser!
}

void	HttpHeader::addEntry(HttpRequest& request, std::string headerLine) {
	HeaderEntry	newEntry;

	newEntry = parseHeader(headerLine);
	request.addHeader(newEntry);
}

HeaderEntry&	HttpHeader::findEntry(HttpRequest& request, std::string name) {
	//TODO implementar si es necesario
}

ValueMap&		HttpHeader::findValue(HeaderEntry& entry, std::string name) {
	//TODO implementar si es necesario
}

PropertyMap&	HttpHeader::findProperty(ValueMap& values, std::string name) {
	//TODO implementar si es necesario
}

std::string&	HttpHeader::findPropertyValue(PropertyMap& property, std::string name) {
	//TODO implementar si es necesario
}

void			HttpHeader::appendHeaders(std::stringstream& buffer, HttpResponse& response) {
	HeaderList&			headers = response.getHeaders();

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