#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class DataAdapter {
private:
	std::string	_buffer;
	HttpRequest	_request;

	DataAdapter(const DataAdapter& src);
	DataAdapter& operator=(const DataAdapter& src);

	void	processRequest();
public:
	DataAdapter();
	~DataAdapter();

	void	recieveData(std::string& request);
	void	sendData(HttpResponse& response);
};
