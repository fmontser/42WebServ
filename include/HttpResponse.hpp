#pragma once
#include "HttpMessage.hpp"

class HttpResponse : public HttpMessage {
	public:
		std::string	statusCode;
		std::string	statusMsg;

		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& src);
		HttpResponse& operator=(const HttpResponse& src);

		bool	isChunked();
};