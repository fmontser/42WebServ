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

	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& src);
		HttpRequest& operator=(const HttpRequest& src);

		void	clear();

		std::string									getMethod() const;
		std::string									getUrl() const;
		std::string									getVersion() const;
		std::multimap<std::string, std::string>		getHeaders() const;
		std::string									getBody() const;

		void	setMethod(const std::string& method);
		void	setUrl(const std::string& method);
		void	setVersion(const std::string& method);
		void	addHeader(std::pair<std::string, std::string>& header);
		void	setBody(const std::string& method);
};

