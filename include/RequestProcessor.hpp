#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Connection.hpp"

class DataAdapater;

class HttpProcessor {
	private:

	public:
		HttpProcessor();
		HttpProcessor(const HttpProcessor& src);
		HttpProcessor& operator=(const HttpProcessor& src);
		~HttpProcessor();

		static void	processHttpRequest(DataAdapter& dataAdapter);
};