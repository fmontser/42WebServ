#pragma once

#include <string>
#include <vector>
#include "HeaderProperty.hpp"

class HeaderValue {
	private:
		void*	operator new(size_t size);
		void	operator delete(void* ptr);
	public:
		std::string					name;
		std::vector<HeaderProperty>	properties;

		HeaderValue();
		HeaderValue(const HeaderValue& src);
		HeaderValue& operator=(const HeaderValue& src);
		~HeaderValue();

		bool	getPropertie(std::string name, HeaderProperty *property);

		void	addProperty(HeaderProperty property);
};