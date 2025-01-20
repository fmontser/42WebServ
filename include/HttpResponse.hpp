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

		void	push(int socketFd);

		void	setVersion(std::string version);
		void	setStatusCode(std::string statusCode);
		void	setStatusMsg(std::string statusMsg);
		void	addHeader(std::string name, std::string value);
		void	setBody(std::string body);
};

