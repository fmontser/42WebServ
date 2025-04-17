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

#define RD_END 0
#define WR_END 1

CgiAdapter::CgiAdapter() {
	_pid = 0;
}
CgiAdapter::~CgiAdapter() {}

CgiAdapter::CgiAdapter(const CgiAdapter& src) {
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
	char		buffer[READ_BUFFER];
	ssize_t		bytesRead;

	if (_pid == 0){
		if (pipe(_pipefdOut) == -1 || pipe(_pipefdIn) == -1)
			return HttpResponse::SERVER_ERROR;
		_startTime = time(NULL);
		_pid = fork();
		if (_pid == -1)
			return HttpResponse::SERVER_ERROR; 
	}

	if (_pid == 0) {
		close(_pipefdIn[WR_END]);
		dup2(_pipefdIn[RD_END], STDIN_FILENO);
 		close(_pipefdOut[RD_END]);
		dup2(_pipefdOut[WR_END], STDOUT_FILENO);
		
		//TODO error
		chdir(_path.c_str());

		int fd = open("log", O_CREAT, 0);
		char	buffer[1000000];
	

		read(_pipefdIn[RD_END], buffer, 8092);
		write(fd, buffer, 8092);
		close(fd);

/* 		if (execve(_path.c_str(), _argv, _envp) == -1) {
			close(_pipefdIn[RD_END]);
			close(_pipefdOut[WR_END]);
			exit(EXIT_FAILURE);
		} */

		sleep(20);
		close(_pipefdIn[RD_END]);
		close(_pipefdOut[WR_END]);
		exit(EXIT_SUCCESS);

	} else {
		close(_pipefdIn[RD_END]);
		close(_pipefdOut[WR_END]);

		if (!dataAdapter.getConnection()->hasPendingCgi) {

			std::string buffer = std::string(body.begin(), body.end());
			write(_pipefdIn[WR_END], buffer.c_str(), buffer.size());
			close(_pipefdIn[WR_END]);
		}

		if (waitpid(_pid, &_waitStatus, WNOHANG) > 0) {
			if (WIFEXITED(_waitStatus)) {
				if (WEXITSTATUS(_waitStatus) == 0) {

					while ((bytesRead = read(_pipefdOut[RD_END], buffer, READ_BUFFER))) {
						if (bytesRead < 0)
							return HttpResponse::SERVER_ERROR;
						output.append(buffer, bytesRead);
					}

					close(_pipefdOut[RD_END]);
					dataAdapter.getConnection()->hasPendingCgi = false;
					return HttpResponse::OK;
				}
				else if (WEXITSTATUS(_waitStatus) != 0) {
					//TODO cerrar pipes
					std::cerr << "Error: CGI script exited with status " << WEXITSTATUS(_waitStatus) << std::endl;
					return HttpResponse::SERVER_ERROR;
				}
			} 
		}
		_actualTime = time(NULL);
		if (_actualTime - _startTime > TIMEOUT) {
			//TODO error or log
			close(_pipefdOut[WR_END]);
			close(_pipefdOut[RD_END]);
			kill(_pid, SIGKILL);
			dataAdapter.getConnection()->hasPendingCgi = false;
			return HttpResponse::GATEWAY_TIMEOUT;
		}

		dataAdapter.getConnection()->hasPendingCgi = true;
		return HttpResponse::EMPTY;
	}
}

//TODO test check errors
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

//TODO manage errors!!!!
void	CgiAdapter::parseParameters(DataAdapter& dataAdapter){
	HttpRequest&	request = dataAdapter.getRequest();
	std::vector<std::string> raw = Utils::splitString(request.url, '?');
	HttpHeader *header = NULL;

	_scriptName = raw[0].substr(0, raw[0].find(".cgi",0) + 4);
	_pathInfo = raw[0].substr(raw[0].find(".cgi",0) + 4, raw[0].size());
	_query = raw[1];
	
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
	return false;
}

std::string	CgiAdapter::stripCgiParams(std::string url) {
	return url.substr(0, url.find('?',0));
}

std::string CgiAdapter::stripCgiPathInfo(std::string url) {
	return url.substr(0, url.find(".cgi",0) + 4);
}