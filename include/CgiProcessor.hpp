#pragma once

#include "HttpResponse.hpp"
#include "DataAdapter.hpp"

class CgiProcessor {
	private:
	
	CgiProcessor();
	~CgiProcessor();
	CgiProcessor(const CgiProcessor& src);
	CgiProcessor& operator=(const CgiProcessor& src);
	
	public:
	static void	processCgi(DataAdapter& dataAdapter);
	static bool	isCgiRequest(std::string url);
};