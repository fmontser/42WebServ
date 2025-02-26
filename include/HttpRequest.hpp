#pragma once

#include "HttpMessage.hpp"
#include "Connection.hpp"

class HttpRequest : public HttpMessage {
	public:
		std::string	method;
		std::string	url;

		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& src);
		HttpRequest& operator=(const HttpRequest& src);

		bool	handleMultipart(Connection *connection);
};

