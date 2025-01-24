#pragma once

#include "HttpResponse.hpp"

class CgiProcessor {
private:

	HttpResponse	_response;

	CgiProcessor(const CgiProcessor& src);
	CgiProcessor& operator=(const CgiProcessor& src);

	void	processHttpResponse();
	void	setEnvironment();
	void	executeCgi();
public:
	CgiProcessor();
	~CgiProcessor();

	void	recieveHttpResponse(HttpResponse& response);
};