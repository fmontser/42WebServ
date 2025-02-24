#include "HttpHeader.hpp"
#include "HttpResponse.hpp"

HttpHeader::HttpHeader(std::string name) {
	this->name = name;
	this->values = values;
}

HttpHeader::HttpHeader(const HttpHeader& src) {
	this->name = src.name;
	this->values = src.values;
}

HttpHeader& HttpHeader::operator=(const HttpHeader& src) {
	if (this != &src) {
		this->name = name;
		this->values = values;
	}
	return *this;
}

HttpHeader::~HttpHeader() {}

bool	HttpHeader::getValue(std::string name, HeaderValue *value) {
	
	(void)value;
	for (std::vector<HeaderValue>::iterator it = values.begin(); it != values.end(); ++it) {
		if (it->name == name) {
			value = &(*it);
			return true;
		}
	}
	return false;
}

void	HttpHeader::addValue(HeaderValue value) {
	values.push_back(value);
}