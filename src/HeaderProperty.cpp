#include "HeaderProperty.hpp"

HeaderProperty::HeaderProperty() {}

HeaderProperty::HeaderProperty(const HeaderProperty& src) {
	this->name = src.name;
	this->value = src.value;
}

HeaderProperty& HeaderProperty::operator=(const HeaderProperty& src) {
	if (this != &src) {
		this->name = src.name;
		this->value = src.value;
	}
	return *this;
}

HeaderProperty::~HeaderProperty() {}

