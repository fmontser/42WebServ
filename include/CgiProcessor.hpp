#pragma once

#include "HttpResponse.hpp"
#include "DataAdapter.hpp"

class CgiProcessor {
	private:
		std::string	_cgiName;
		std::string	_method;
		std::string	_query;
		std::string	_cType;
		std::string	_cLength;
		
		char		*_envp[5];
		char		*_argv[2];

		std::string	executeCgi();
		void		parseParameters(std::string url);
		void		setEnvironment(DataAdapter& dataAdapter);

	public:
		CgiProcessor();
		~CgiProcessor();
		CgiProcessor(const CgiProcessor& src);
		CgiProcessor& operator=(const CgiProcessor& src);

		void		processCgi(DataAdapter& dataAdapter);
		static bool	isCgiRequest(std::string url);
};