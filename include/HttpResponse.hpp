#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpResponse {
	private:

		std::string								version;
		std::string								statusCode;
		std::string								statusMsg;
		std::multimap<std::string, std::string>	headers;
		std::string								body;

	public:
		HttpResponse();
		~HttpResponse();

		void	push();

		void	setVersion(std::string version);
		void	setStatusCode(std::string statusCode);
		void	setStatusMsg(std::string statusMsg);
		void	addHeader(std::string name, std::string value);
		void	setBody(std::string body);
};

