#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpRequest {
	private:
		std::stringstream						*_request;
		std::string								_method;
		std::string								_url;
		std::string								_version;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;
		int										_socket;

		void	parseRequestLine();
		void	parseHeaderFields();
		void	parseBody();
		void	trimToken(std::string& str);

	public:
		HttpRequest();
		~HttpRequest();

		void	pull(std::stringstream *request, int socketFd);

		std::string								getMethod() const;
		std::string								getUrl() const;
		std::string								getVersion() const;
		std::multimap<std::string, std::string>	getHeaders() const;
		std::string								getBody() const;
		int										getSocket() const;
	//	std::string to_string(int value);
};

