#include <unistd.h>
#include <iostream>
#include <map>
#include <cstdlib>
#include <wait.h>
#include "CgiAdapter.hpp"
#include "Utils.hpp"
#include "ServerConstants.hpp"

#define RD_PIPE 0
#define WR_PIPE 1

CgiAdapter::CgiAdapter() {}
CgiAdapter::~CgiAdapter() {}

CgiAdapter::CgiAdapter(const CgiAdapter& src) {
	_cgiName = src._cgiName;
	_method = src._method;
	_query = src._query;
	_cType = src._cType;
	_cLength = src._cLength;
}

CgiAdapter& CgiAdapter::operator=(const CgiAdapter& src) {
	if (this != &src){
		_cgiName = src._cgiName;
		_method = src._method;
		_query = src._query;
		_cType = src._cType;
		_cLength = src._cLength;
	}
	return *this;
}

#include <cerrno>
#include <cstring>

HttpResponse::responseType	CgiAdapter::executeCgi(std::string& output) {
	std::string	path;
	pid_t		pid;
	int			pipefd[2];
	int			waitStatus, pipeStatus;
	char		buffer[READ_BUFFER];
	ssize_t		bytesRead;

	path.append("cgi-bin/");
	path.append(_cgiName);

	pipeStatus = pipe(pipefd);
	if (pipeStatus == -1)
		return HttpResponse::SERVER_ERROR;

	pid = fork();
	if (pid == -1)
		return HttpResponse::SERVER_ERROR; 

	if (pid == 0) {
 		close(pipefd[RD_PIPE]);
		dup2(pipefd[WR_PIPE], STDOUT_FILENO);
		
		if (execve(path.c_str(), _argv, _envp) == -1){
			close(pipefd[WR_PIPE]);
			exit(EXIT_FAILURE);
		}
		close(pipefd[WR_PIPE]);
		exit(EXIT_SUCCESS);

	} else {
		close(pipefd[WR_PIPE]);

		while ((bytesRead = read(pipefd[RD_PIPE], buffer, READ_BUFFER))) {
			if (bytesRead < 0)
				return HttpResponse::SERVER_ERROR;
			output.append(buffer, bytesRead);
		}

		waitpid(pid, &waitStatus, WNOHANG);
		if (WIFEXITED(waitStatus)) {
			if (WEXITSTATUS(waitStatus) == 0) {
				close(pipefd[RD_PIPE]);
				return HttpResponse::OK;
			}
			if (WEXITSTATUS(waitStatus) != 0) {
				std::cerr << "Error: CGI script exited with status " << WEXITSTATUS(waitStatus) << std::endl;
				return HttpResponse::SERVER_ERROR;
			}
		} 
		return HttpResponse::EMPTY;
	}
}

//TODO test check errors
void	CgiAdapter::setEnvironment(DataAdapter& dataAdapter) {
	HttpRequest	request = dataAdapter.getRequest();
	HttpHeader	*cTypeHeader = request.findHeader("Content-Type");
	HttpHeader	*cLengthHeader = request.findHeader("Content-Length");
	HeaderValue	value;
	int			i = 0;
	
	_argv[i] = const_cast<char *>(_cgiName.c_str());
	_argv[i+1] = NULL;

	_method = std::string("REQUEST_METHOD=").append(request.method);
	_envp[i++] = const_cast<char *>(_method.c_str());

	_query = std::string("QUERY_STRING=").append(_query);
	_envp[i++] = const_cast<char *>(_query.c_str());

	//TODO como comprobar?
	if (cTypeHeader){
		cTypeHeader->getValue("Content-Type", &value);
	}
	_cType = std::string("CONTENT_TYPE=").append(value.name.c_str());
	_envp[i++] = const_cast<char *>(_cType.c_str());

	//TODO como comprobar?
	if (cLengthHeader){
		cLengthHeader->getValue("Content-Length", &value);
	}
	_cLength = std::string("CONTENT_LENGTH=").append(value.name.c_str());
	_envp[i] = const_cast<char *>(_cLength.c_str());

	_envp[i+1] = NULL;
}

//TODO manage errors!!!!
void	CgiAdapter::parseParameters(std::string url){
	std::vector<std::string> raw = Utils::splitString(url, '?');

	_cgiName = raw[0].substr(
		raw[0].find_last_of('/', raw[0].size()) + 1
		, raw[0].size()
	);
	_query = raw[1];
}

HttpResponse::responseType	CgiAdapter::processCgi(DataAdapter& dataAdapter) {
	std::string					output;
	HttpResponse::responseType	responseType;
	size_t						i = 0;

	parseParameters(dataAdapter.getRequest().url);
	setEnvironment(dataAdapter);
	responseType = executeCgi(output);
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