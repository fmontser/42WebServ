#pragma once

#include <cstdlib>
#include "HttpResponse.hpp"
#include "DataAdapter.hpp"

class CgiAdapter {
	private:
		std::string	_scriptName;
		std::string	_method;
		std::string	_query;
		std::string	_pathInfo;
		std::string	_cType;
		std::string	_cLength;
		std::string	_path;
		
		time_t		_startTime;
		time_t		_actualTime;
		
		int			_waitStatus;
		pid_t		_pid;
		int			_ppIn[2];
		int			_ppOut[2];

		char		*_envp[7];
		char		*_argv[3];

		HttpResponse::responseType	executeCgi(std::string& output, DataAdapter& dataAdapter);
		void						parseParameters(DataAdapter& dataAdapter);
		void						setEnvironment(DataAdapter& dataAdapter);

	public:
		CgiAdapter();
		~CgiAdapter();
		CgiAdapter(const CgiAdapter& src);
		CgiAdapter& operator=(const CgiAdapter& src);

		std::vector<char>	body;

		HttpResponse::responseType	processCgi(DataAdapter& dataAdapter);
		static bool	isCgiRequest(std::string url);
		static std::string	stripCgiParams(std::string url);
		static std::string	stripCgiPathInfo(std::string url);
};