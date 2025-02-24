#pragma once

#include <string>
#include <vector>

class HeaderProperty {
	public:
		std::string	name;
		std::string	value;

		HeaderProperty(std::string name, std::string value);
		HeaderProperty(const HeaderProperty& src);
		HeaderProperty& operator=(const HeaderProperty& src);
		~HeaderProperty();
};