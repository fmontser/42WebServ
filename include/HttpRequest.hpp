#pragma once

#include "HttpMessage.hpp"

class Connection;

class HttpRequest : public HttpMessage {
	public:
		std::string	method;
		std::string	url;

		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& src);
		HttpRequest& operator=(const HttpRequest& src);

		bool	isBinaryDownload;
		bool	handleMultipart(Connection *connection);
		std::string getCleanHost() const;
};

