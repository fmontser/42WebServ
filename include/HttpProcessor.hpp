#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Connection.hpp"

class DataAdapater;

class HttpProcessor {
	public:
		HttpProcessor();
		HttpProcessor(const HttpProcessor& src);
		HttpProcessor& operator=(const HttpProcessor& src);
		~HttpProcessor();

		static void	processHttpRequest(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter);
};