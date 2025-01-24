#pragma once

#include "HttpResponse.hpp"

class CgiProcessor {
	private:
		static HttpResponse	_response;

		CgiProcessor();
		~CgiProcessor();
		CgiProcessor(const CgiProcessor& src);
		CgiProcessor& operator=(const CgiProcessor& src);

		static void	processHttpResponse();
		static void	setEnvironment();
		static void	executeCgi();
	public:
		static void	recieveHttpResponse(HttpResponse& response);
};