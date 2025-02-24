#pragma once

#include <string>
#include <vector>
#include "HeaderValue.hpp"

class HttpResponse;

class HttpHeader {
	public:
		std::string					name;
		std::vector<HeaderValue>	values;

		HttpHeader(std::string name);
		HttpHeader(const HttpHeader& src);
		HttpHeader& operator=(const HttpHeader& src);
		~HttpHeader();

		bool	getValue(std::string name, HeaderValue *value);

		void		addValue(HeaderValue value);
};