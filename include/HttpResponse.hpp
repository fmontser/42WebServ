#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpResponse {
	private:
		std::string								_version;
		std::string								_statusCode;
		std::string								_statusMsg;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
	public:
		HttpResponse();
		~HttpResponse();
		HttpResponse(const HttpResponse& src);
		HttpResponse& operator=(const HttpResponse& src);

		std::string								getVersion() const;
		std::string								getStatusCode() const;
		std::string								getStatusMsg() const;
		std::multimap<std::string, std::string>	getHeaders() const;
		std::string								getBody() const;

		void	setVersion(const std::string version);
		void	setStatusCode(const std::string statusCode);
		void	setStatusMsg(const std::string statusMsg);
		void	addHeader(const std::pair<std::string, std::string>& header);
		void	setBody(const std::string body);
};