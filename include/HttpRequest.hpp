#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpRequest {
	private:
		std::stringstream						*request;
		std::string								method;
		std::string								url;
		std::string								version;
		std::multimap<std::string, std::string>	headers;
		std::string								body;

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

