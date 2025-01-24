#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class DataAdapter {
private:
	std::string	_buffer;
	HttpRequest	_request;

	void	processRequest();
public:
	DataAdapter();
	~DataAdapter();
	DataAdapter(const DataAdapter& src);
	DataAdapter& operator=(const DataAdapter& src);

	void	recieveData(std::string& request);
	void	sendData(HttpResponse& response);
};
