#pragma once
#include <string>
#include <sstream>
#include <map>
#include "HttpHeader.hpp"

class HttpResponse {
	private:
		std::string								_version;
		std::string								_statusCode;
		std::string								_statusMsg;
		HeaderList								_headers;
		std::string								_body;
	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& src);
		HttpResponse& operator=(const HttpResponse& src);

		void	clear();

		std::string								getVersion() const;
		std::string								getStatusCode() const;
		std::string								getStatusMsg() const;
		HeaderList&								getHeaders();
		std::string								getBody() const;

		void	setVersion(const std::string version);
		void	setStatusCode(const std::string statusCode);
		void	setStatusMsg(const std::string statusMsg);
		void	addHeader(HeaderEntry header);
		void	setBody(const std::string body);
};