#include "CgiProcessor.hpp"
#include "Utils.hpp"
#include <unistd.h>
#include <iostream>
#include <map>
#include <cstdlib>
#include <wait.h>

CgiProcessor::CgiProcessor() {}
CgiProcessor::~CgiProcessor() {}

CgiProcessor::CgiProcessor(const CgiProcessor& src) {
	_cgiName = src._cgiName;
	_method = src._method;
	_query = src._query;
	_cType = src._cType;
	_cLength = src._cLength;
}

CgiProcessor& CgiProcessor::operator=(const CgiProcessor& src) {
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

std::string	CgiProcessor::executeCgi() {
	std::string	path, output;

	path.append("cgi-bin/");
	path.append(_cgiName);

	

	pid_t pid;



	pid = fork();
	if (pid < 0) {
		std::cerr << "Error: fork failed: " << std::endl;
		//TODO 500
		return output;
	}
	
	if (pid == 0) { // ############################ CHILD!!!!!!

		if (execve(path.c_str(), _argv, _envp) == -1) {
			std::cerr << "Error: execve failed: " << std::endl;
			std::cerr << "errno: " << errno << std::endl; // Imprimir el valor de errno
			std::cerr << "strerror: " << strerror(errno) << std::endl; // Imprimir el mensaje de error asociado
			exit(EXIT_FAILURE);
		}
		



	
	} else { // ############################ PARENT!!!!!!

		
/* 		char buffer[4096];
		ssize_t bytesRead;
		
		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer)))) {
			if (bytesRead < 0) {
				std::cerr << "Error reading from CGI script: " << std::endl;
				break;
			}
			output.append(buffer, bytesRead);
		}
		close(outputPipe[0]); */

/* 		int status;
		waitpid(pid, &status, 0);
		
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
				//TODO 500
			}
		} else if (WIFSIGNALED(status)) {
			std::cerr << "CGI script terminated by signal " << WTERMSIG(status) << std::endl;
			//TODO 500
		} */
		return output;
	}
	return output;
}

//TODO test check errors
void	CgiProcessor::setEnvironment(DataAdapter& dataAdapter) {
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
void	CgiProcessor::parseParameters(std::string url){
	std::vector<std::string> raw = Utils::splitString(url, '?');

	_cgiName = raw[0].substr(
		raw[0].find_last_of('/', raw[0].size()) + 1
		, raw[0].size()
	);
	//TODO  check añadir comillas simples o dobles???? contiene (=) iguales! (es valido en principio)
	_query = raw[1];
}

void	CgiProcessor::processCgi(DataAdapter& dataAdapter) {
	parseParameters(dataAdapter.getRequest().url);
	setEnvironment(dataAdapter);
	executeCgi();
}

bool	CgiProcessor::isCgiRequest(std::string url) {
	if (url.find(".cgi", 0) != url.npos)
		return true;
	return false;
}

/* 

REQUEST_METHOD: Indica el método HTTP utilizado en la solicitud (GET, POST, HEAD, etc.). Necesitas saber esto para determinar cómo procesar la solicitud y los datos que pueda contener.

QUERY_STRING: Contiene la parte de la URL que sigue al signo de interrogación ?. Esta cadena contiene los parámetros que se pasan al script CGI en una solicitud GET. Necesitas parsear esta cadena para obtener los valores de los parámetros.

CONTENT_TYPE (solo para POST): Indica el tipo de medio de los datos enviados en el cuerpo de la solicitud (por ejemplo, application/x-www-form-urlencoded, multipart/form-data). Es importante para saber cómo decodificar los datos enviados por el cliente.

CONTENT_LENGTH (solo para POST): Indica la longitud en bytes de los datos enviados en el cuerpo de la solicitud. Necesitas esta información para leer la cantidad correcta de datos del flujo de entrada.


*/


/* 
void	CgiProcessor::executeCgi(const std::string& cgiPath, const std::string& input) {
	int inputPipe[2];
	int outputPipe[2];
	pid_t pid;
	
	if (pipe(inputPipe) < 0 || pipe(outputPipe) < 0) {
		std::cerr << "Error: pipe failed: " << strerror(errno) << std::endl;
		_response.setBody("Error executing CGI script: pipe failed");
		return;
	}
	
	pid = fork();
	if (pid < 0) {
		std::cerr << "Error: fork failed: " << strerror(errno) << std::endl;
		_response.setBody("Error executing CGI script: fork failed");
		return;
	}
	
	if (pid == 0) { // Child process
		close(inputPipe[1]); // Close write end of input pipe
		close(outputPipe[0]); // Close read end of output pipe
		
		// Redirect stdin and stdout
		dup2(inputPipe[0], STDIN_FILENO);
		dup2(outputPipe[1], STDOUT_FILENO);
		
		// Close the duplicated file descriptors
		close(inputPipe[0]);
		close(outputPipe[1]);
		
		// Prepare arguments for execve
		char *argv[] = {const_cast<char*>(cgiPath.c_str()), NULL};
		
		// Execute the CGI script
		execve(cgiPath.c_str(), argv, environ);
		
		// If execve fails
		std::cerr << "Error: execve failed: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	} else { // Parent process
		close(inputPipe[0]); // Close read end of input pipe
		close(outputPipe[1]); // Close write end of output pipe
		
		// Write input to CGI script if this is a POST request
		if (!input.empty()) {
			write(inputPipe[1], input.c_str(), input.size());
		}
		close(inputPipe[1]);
		
		char buffer[4096];
		ssize_t bytesRead;
		std::string output;
		
		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer)))) {
			if (bytesRead < 0) {
				std::cerr << "Error reading from CGI script: " << strerror(errno) << std::endl;
				break;
			}
			output.append(buffer, bytesRead);
		}
		close(outputPipe[0]);

		int status;
		waitpid(pid, &status, 0);
		
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				std::cerr << "CGI script exited with status " << WEXITSTATUS(status) << std::endl;
				_response.setBody("Error executing CGI script: non-zero exit status");
			} else {
				_response.setBody(output);
			}
		} else if (WIFSIGNALED(status)) {
			std::cerr << "CGI script terminated by signal " << WTERMSIG(status) << std::endl;
			_response.setBody("Error executing CGI script: terminated by signal");
		}
	}

}




*/