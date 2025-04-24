#pragma once

#include "HttpMessage.hpp"

class Connection;

class HttpRequest : public HttpMessage {
	private:

		bool	handleMultipart(Connection *connection);
		bool	handleChunked(Connection *connection);

	public:
		std::string	method;
		std::string	url;

		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& src);
		HttpRequest& operator=(const HttpRequest& src);

		bool	isBinaryDownload;
		bool	isCgiRequest;
		bool	handlePostMode(Connection *connection);
		std::string getHostName() const;
};

