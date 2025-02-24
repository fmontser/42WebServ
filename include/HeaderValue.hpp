#pragma once

#include <string>
#include <vector>
#include "HeaderProperty.hpp"

class HeaderValue {
	public:
		std::string					name;
		std::vector<HeaderProperty>	properties;

		HeaderValue(std::string name);
		HeaderValue(const HeaderValue& src);
		HeaderValue& operator=(const HeaderValue& src);
		~HeaderValue();

		bool	getPropertie(std::string name, HeaderProperty *property);

		void	addProperty(HeaderProperty property);
};