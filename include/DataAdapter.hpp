#pragma once

#include <string>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"

class DataAdapter {
	private:
		static std::string	_buffer;
		static HttpRequest	_request;
		static Socket&		_targetSocket;

		DataAdapter();
		~DataAdapter();
		DataAdapter(const DataAdapter& src);
		DataAdapter& operator=(const DataAdapter& src);
	public:
		static void					recieveData(Socket *targetSocket, std::string& request);
		static void					sendData(Socket *targetSocket, HttpResponse& response);
		static void 				parseRequestBody(std::stringstream& data, HttpRequest& request, const std::string& contentType);
		static std::string	extractBoundary(const std::string& contentType);
		static std::string	parseMultipartData(const std::string& body, const std::string& boundary);
	//	static std::string	decodeURLEncoded(const std::string& str);
};