#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpRequest {
	private:
		std::string								_method;
		std::string								_url;
		std::string								_version;
		std::multimap<std::string, std::string>	_headers;
		std::string								_body;

		HttpRequest(const HttpRequest& src);
		HttpRequest& operator=(const HttpRequest& src);

		void	parseRequestLine();
		void	parseHeaderFields();
		void	parseBody();
		void	trimToken(std::string& str);

	public:
		HttpRequest();
		~HttpRequest();

		std::string								getMethod() const;
		std::string								getUrl() const;
		std::string								getVersion() const;
		std::multimap<std::string, std::string>	getHeaders() const;
		std::string								getBody() const;

		void									populate(std::stringstream *data);
};

