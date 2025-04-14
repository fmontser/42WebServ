#include "CgiProcessor.hpp"

HttpResponse	CgiProcessor::_response;

CgiProcessor::CgiProcessor() {
	//TODO
}

CgiProcessor::~CgiProcessor() {
	//TODO
}

CgiProcessor::CgiProcessor(const CgiProcessor& src) {
	//TODO
	(void)src;
}

CgiProcessor& CgiProcessor::operator=(const CgiProcessor& src) {
	//TODO
	(void)src;
	return *this;
}


void	CgiProcessor::processHttpResponse() {
	//TODO
}

void	CgiProcessor::setEnvironment() {
	//TODO
}

void	CgiProcessor::executeCgi() {
	//TODO
}

void	CgiProcessor::processCgi(DataAdapter& dataAdapter) {
	
	(void)dataAdapter;
}

bool	CgiProcessor::isCgiRequest(std::string url) {
	//TODO check
	if (url.find(".cgi", 0) != url.npos)
		return true;
	return false;
}

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