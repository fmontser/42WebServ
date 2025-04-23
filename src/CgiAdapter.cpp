#include <iostream>
#include <map>
#include <cstdlib>
#include <wait.h>
#include "CgiAdapter.hpp"
#include "Utils.hpp"
#include "ServerConstants.hpp"
#include "PathManager.hpp"

#include <fcntl.h>
#include <unistd.h>

#define RD 0
#define WR 1

CgiAdapter::CgiAdapter() {
	randomcount = 0;//TODO DELETE
	_initFlag = true;
}
CgiAdapter::~CgiAdapter() {}

CgiAdapter::CgiAdapter(const CgiAdapter& src) {
	_initFlag =  src._initFlag;
	_scriptName = src._scriptName;
	_method = src._method;
	_query = src._query;
	_pathInfo = src._pathInfo;
	_cType = src._cType;
	_cLength = src._cLength;
	_waitStatus = src._waitStatus;
	_path = src._path;
}

CgiAdapter& CgiAdapter::operator=(const CgiAdapter& src) {
	if (this != &src){
		_initFlag =  src._initFlag;
		_scriptName = src._scriptName;
		_method = src._method;
		_query = src._query;
		_pathInfo = src._pathInfo;
		_cType = src._cType;
		_cLength = src._cLength;
		_waitStatus = src._waitStatus;
		_path = src._path;
	}
	return *this;
}

HttpResponse::responseType	CgiAdapter::executeCgi(std::string& output, DataAdapter& dataAdapter) {
	char		readBuffer[READ_BUFFER];
	ssize_t		bytesRead;

	if (_initFlag){
		if (pipe(_ppOut) == -1 || pipe(_ppIn) == -1)
			return HttpResponse::SERVER_ERROR;
		_startTime = time(NULL);
		_pid = fork();
		if (_pid == -1)
			return HttpResponse::SERVER_ERROR; 
	}

	if (_pid == 0) {
		close(_ppIn[WR]);
		dup2(_ppIn[RD], STDIN_FILENO);
		close(_ppIn[RD]);

 		close(_ppOut[RD]);
		dup2(_ppOut[WR], STDOUT_FILENO);
		close(_ppOut[WR]);
	
		chdir(Utils::getPathDir(_path).c_str());
		if (execve(_path.c_str(), _argv, _envp) == -1)
			exit(EXIT_FAILURE);
	} else {

		if (_initFlag) {
			_initFlag = false;
			close(_ppIn[RD]);
			close(_ppOut[WR]);	
			std::string writeBuffer = std::string(body.begin(), body.end());
			if (write(_ppIn[WR], writeBuffer.c_str(), writeBuffer.size())== -1)
				return HttpResponse::SERVER_ERROR;
			close(_ppIn[WR]);
		}

		_actualTime = time(NULL);
		if (_actualTime - _startTime > TIMEOUT) {
			close(_ppOut[RD]);
			kill(_pid, SIGKILL);
			dataAdapter.getConnection()->hasPendingCgi = false;
			return HttpResponse::GATEWAY_TIMEOUT;
		}

		if (waitpid(_pid, &_waitStatus, WNOHANG) > 0) {
			if (WIFEXITED(_waitStatus)) {
				if (WEXITSTATUS(_waitStatus) == 0) {

 					while ((bytesRead = read(_ppOut[RD], readBuffer, READ_BUFFER)))
						output.append(readBuffer, bytesRead);
					if (bytesRead == -1)
						return HttpResponse::SERVER_ERROR;

					close(_ppOut[RD]);
					dataAdapter.getConnection()->hasPendingCgi = false;
					return HttpResponse::OK;
				}
				else if (WEXITSTATUS(_waitStatus) != 0) {
					close(_ppOut[RD]);
					std::cerr << "Error: CGI script exited with status " << WEXITSTATUS(_waitStatus) << std::endl;
					return HttpResponse::SERVER_ERROR;
				}
			}
		}
	}
	dataAdapter.getConnection()->hasPendingCgi = true;
	return HttpResponse::EMPTY;
}

void	CgiAdapter::setEnvironment(DataAdapter& dataAdapter) {
	HttpRequest&	request = dataAdapter.getRequest();
	int				i = 0;
	
	_path = PathManager::resolveServerPath(dataAdapter);

	_argv[i] = const_cast<char *>(_scriptName.c_str());
	_argv[i+1] = const_cast<char *>(_path.c_str());
	_argv[i+2] = NULL;

	_scriptName = std::string("SCRIPT_NAME=").append(_scriptName);
	_envp[i++] = const_cast<char *>(_scriptName.c_str());

	_method = std::string("REQUEST_METHOD=").append(request.method);
	_envp[i++] = const_cast<char *>(_method.c_str());

	_query = std::string("QUERY_STRING=").append(_query);
	_envp[i++] = const_cast<char *>(_query.c_str());

	_pathInfo = std::string("PATH_INFO=").append(_pathInfo);
	_envp[i++] = const_cast<char *>(_pathInfo.c_str());

	_cType = std::string("CONTENT_TYPE=").append(_cType.c_str());
	_envp[i++] = const_cast<char *>(_cType.c_str());

	_cLength = std::string("CONTENT_LENGTH=").append(_cLength.c_str());
	_envp[i] = const_cast<char *>(_cLength.c_str());

	_envp[i+1] = NULL;
}

void	CgiAdapter::parseParameters(DataAdapter& dataAdapter){
	HttpRequest&	request = dataAdapter.getRequest();
	std::vector<std::string> raw = Utils::splitString(request.url, '?');
	HttpHeader *header = NULL;

	_scriptName = raw[0].substr(0, raw[0].find(".cgi",0) + 4);
	_pathInfo = raw[0].substr(raw[0].find(".cgi",0) + 4, raw[0].size());
	_query = raw[1];
	/*
	std::cerr << "this is the " << randomcount << "st/nd/rd/th time"<< std::endl;
	std::cerr << "results of all these finds: " << std::endl;
	std::cerr << "scr name: " << _scriptName << std::endl;
	std::cerr << "pinfo   : " << _pathInfo << std::endl;
	std::cerr << "q       : " << _query << std::endl;
	randomcount++;
	*/ // akozin special xdddd TODO DELETE

	
	header = request.findHeader("Content-Type");
	if (header)
		_cType = header->values[0].name;
	header = request.findHeader("Content-Length");
	if (header)
		_cLength = header->values[0].name;
}

HttpResponse::responseType	CgiAdapter::processCgi(DataAdapter& dataAdapter) {
	std::string					output;
	HttpResponse::responseType	responseType;
	size_t						i = 0;

	parseParameters(dataAdapter);
	setEnvironment(dataAdapter);
	responseType = executeCgi(output, dataAdapter);
	if (responseType != HttpResponse::EMPTY) {
		while (i < output.size())
			dataAdapter.getResponse().body.push_back(output[i++]);
	}
	return responseType;
}

bool	CgiAdapter::isCgiRequest(std::string url) {
	if (url.find(".cgi", 0) != url.npos)
		return true;
	if (url.find(".bla", 0) != url.npos)
		return true;
	return false;
}

std::string	CgiAdapter::stripCgiParams(std::string url) {
	return url.substr(0, url.find('?',0));
}

std::string CgiAdapter::stripCgiPathInfo(std::string url) {
	return url.substr(0, url.find(".cgi",0) + 4);
}