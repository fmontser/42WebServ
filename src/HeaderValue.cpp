#include "HeaderValue.hpp"

HeaderValue::HeaderValue(std::string name) {
	this->name = name;
	this->properties = properties;
}

HeaderValue::HeaderValue(const HeaderValue& src) {
	this->name = src.name;
	this->properties = src.properties;
}

HeaderValue& HeaderValue::operator=(const HeaderValue& src) {
	if (this != &src) {
		this->name = name;
		this->properties = properties;
	}
	return *this;
}

HeaderValue::~HeaderValue() {}

bool	HeaderValue::getPropertie(std::string name, HeaderProperty *property) {
	(void)property;
	for (std::vector<HeaderProperty>::iterator it = properties.begin(); it != properties.end(); ++it) {
		if (it->name == name) {
			property = &(*it);
			return true;
		}
	}
	return false;
}

void	HeaderValue::addProperty(HeaderProperty property) {
	properties.push_back(property);
}

