#pragma once

#include <string>
#include <vector>
#include "HttpHeader.hpp"

class HttpMessage {
	private:
		void*	operator new(size_t size);
		void	operator delete(void* ptr);

	protected:
		HttpMessage();
		~HttpMessage();
		HttpMessage(const HttpMessage& src);
		HttpMessage& operator=(const HttpMessage& src);

	public:
		std::string				version;
		std::vector<HttpHeader>	headers;
		std::vector<char>		body;

		void	addHeader(HttpHeader header);
		void	addHeader(std::string header);
};