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

		void	parseRequestLine();
		void	parseHeaderFields();
		void	parseBody();
		void	trimToken(std::string& str);

	public:
		HttpRequest();
		~HttpRequest();

		void	pull(std::stringstream *request);

		std::string								getMethod();
		std::string								getUrl();
		std::string								getVersion();
		std::multimap<std::string, std::string>	getHeaders();
		std::string								getBody();
};

