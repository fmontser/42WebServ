#pragma once

#include <string>
#include <vector>
#include "HeaderValue.hpp"

class HttpResponse;

class HttpHeader {
	private:
		void*	operator new(size_t size);
		void	operator delete(void* ptr);
	public:
		std::string					name;
		std::vector<HeaderValue>	values;

		HttpHeader();
		HttpHeader(const HttpHeader& src);
		HttpHeader& operator=(const HttpHeader& src);

		~HttpHeader();

		bool	getValue(std::string name, HeaderValue *value);

		void		addValue(HeaderValue value);
};