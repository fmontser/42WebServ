#pragma once
#include <string>
#include <sstream>
#include <map>

class HttpReq {
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
		HttpReq(std::stringstream *request);
		~HttpReq();
};

