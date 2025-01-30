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
		static void	recieveData(Socket& targetSocket, std::string& request);
		static void	sendData(Socket& targetSocket, HttpResponse& response);
};
