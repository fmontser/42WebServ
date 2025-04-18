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
	char		buffer[1000000]; //TODO hacer reserva dinamica en funcion del content size...
	ssize_t		bytesRead;

	if (_pid == 0){
		if (pipe(_ppOut) == -1 || pipe(_ppIn) == -1)
			return HttpResponse::SERVER_ERROR;
		_startTime = time(NULL);
		_pid = fork();
		if (_pid == -1)
			return HttpResponse::SERVER_ERROR; 
	}

	if (_pid == 0) {
		close(_ppIn[WR]);
		close(_ppIn[RD]);
		//dup2(_ppIn[RD], STDIN_FILENO);
 		close(_ppOut[RD]);
		dup2(_ppOut[WR], STDOUT_FILENO);
		
		//TODO error
		//chdir(_path.c_str());

		if (execve(_path.c_str(), _argv, _envp) == -1) {
			close(_ppOut[WR]);
			exit(EXIT_FAILURE);
		}

		close(_ppOut[WR]);
		exit(EXIT_SUCCESS);

	} else {
		close(_ppIn[WR]);
		close(_ppIn[RD]);
		close(_ppOut[WR]);

		if (waitpid(_pid, &_waitStatus, WNOHANG) > 0) {
			if (WIFEXITED(_waitStatus)) {
				if (WEXITSTATUS(_waitStatus) == 0) {

					while ((bytesRead = read(_ppOut[RD], buffer, READ_BUFFER))) {
						if (bytesRead < 0)
							return HttpResponse::SERVER_ERROR;
						output.append(buffer, bytesRead);
					}

					close(_ppOut[RD]);
					dataAdapter.getConnection()->hasPendingCgi = false;
					return HttpResponse::OK;
				}
				else if (WEXITSTATUS(_waitStatus) != 0) {
					//TODO cerrar pipes
					close(_ppOut[RD]);
					std::cerr << "Error: CGI script exited with status " << WEXITSTATUS(_waitStatus) << std::endl;
					return HttpResponse::SERVER_ERROR;
				}
			} 
		}

		_actualTime = time(NULL);
		if (_actualTime - _startTime > TIMEOUT) {
			//TODO error or log

			close(_ppOut[RD]);
			kill(_pid, SIGKILL);
			dataAdapter.getConnection()->hasPendingCgi = false;
			return HttpResponse::GATEWAY_TIMEOUT;
		}

		dataAdapter.getConnection()->hasPendingCgi = true;
		return HttpResponse::EMPTY;
	}
}





/* 

		if (!dataAdapter.getConnection()->hasPendingCgi) {

			std::string buffer = std::string(body.begin(), body.end());
			write(_ppIn[WR], buffer.c_str(), buffer.size());
			close(_ppIn[WR]);
		}




#include <iostream>
#include <unistd.h>
#include <wait.h>

#define RD 0
#define WR 1

int main(void) {

	char buffer[11] = {0};
	int	pIn[2], pOut[2];
	pid_t pid;

	if (pipe(pIn) == -1 || pipe(pOut) == -1)
		std::cerr << "Error: Pipe error\n";
	pid = fork();
	if (pid == -1)
		std::cerr << "Error: Fork error\n";

	if (pid == 0) {
	// CHILD CODE

		close(pOut[RD]);
		dup2(pOut[WR], STDOUT_FILENO);

		close(pIn[WR]);
		dup2(pIn[RD], STDIN_FILENO);

		read(pIn[RD], buffer, 11);

		buffer[0] = 'X';

		write(pOut[WR], buffer, 11);

		close(pOut[WR]);
		close(pIn[RD]);

	} else {
	// PARENT CODE

		close(pOut[WR]);
		close(pIn[RD]);

		write(pIn[WR], "0123456789\0", 11);
		close(pIn[WR]);

		waitpid(pid, NULL, 0);

		read(pOut[RD], buffer, 11);
		close(pOut[RD]);

		std::cout << buffer << "\n";
	}
	return 0;
}

*/

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