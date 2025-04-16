#pragma once

#include <unistd.h>
#include "HttpResponse.hpp"
#include "DataAdapter.hpp"

class CgiAdapter {
	private:
		std::string	_cgiName;
		std::string	_method;
		std::string	_query;
		std::string	_cType;
		std::string	_cLength;
		
		int			_waitStatus;
		pid_t		_pid;
		int			_pipefd[2];

		char		*_envp[5];
		char		*_argv[2];

		HttpResponse::responseType	executeCgi(std::string& output, DataAdapter& dataAdapter);
		void						parseParameters(std::string url);
		void						setEnvironment(DataAdapter& dataAdapter);

	public:
		CgiAdapter();
		~CgiAdapter();
		CgiAdapter(const CgiAdapter& src);
		CgiAdapter& operator=(const CgiAdapter& src);

		HttpResponse::responseType	processCgi(DataAdapter& dataAdapter);
		static bool	isCgiRequest(std::string url);
		static std::string	stripCgiParams(std::string url);
};