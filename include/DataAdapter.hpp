#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class DataAdapter {
	private:
		static std::string	_buffer;
		static HttpRequest	_request;

		DataAdapter();
		~DataAdapter();
		DataAdapter(const DataAdapter& src);
		DataAdapter& operator=(const DataAdapter& src);

		static void	processRequest();
	public:
		static void	recieveData(std::string& request);
		static void	sendData(HttpResponse& response);
};
