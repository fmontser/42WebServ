#include "RequestProcessor.hpp"
#include "DataAdapter.hpp"

void	HttpProcessor::processHttpRequest(DataAdapter& dataAdapter) {
	int	fd;
	HttpRequest& _request = dataAdapter.getRequest();
	HttpResponse& _response = dataAdapter.getResponse();

	//TODO @@@@@@@@ continuar aqui!!!!!! @@@@@@@@@@@@

	_response.setVersion(HTTP_VERSION);
	_response.addHeader(std::make_pair("Connection", "keep-alive"));
	if (_request.getMethod() == "GET") {
		fd = readFile(socket, _request, _response);
		if (_response.getStatusCode().empty()) {
			_response.setStatusCode("200");
			_response.setStatusMsg("OK");
			std::cout << BLUE << "Info: success 200 \"" << _request.getMethod() << "\", OK " << " FD: " << socket->getFd() << END << std::endl;
		}
	}
	else if (_request.getMethod() == "POST") {

		//TODO set boundarie....hardcoded
		if (socket->boundarie.empty()) {
			std::string bound = _request.getHeaders().find("Content-Type")->second;
			std::string len = _request.getHeaders().find("Content-Length")->second;
			socket->contentLength = atoi(len.c_str());
			socket->boundarie = bound.substr(bound.find('=') + 1, bound.size());
			socket->multiMode = true;
		}


	}
	else if (_request.getMethod() == "DELETE") {
		//TODO DELETE METHOD
		_response.setStatusCode("204");
		_response.setStatusMsg("NO_CONTENT");
		std::cout << BLUE << "Info: success 204 \"" << _request.getMethod() << "\", NO_CONTENT " << END << std::endl;
	}
	else {

/* 		_request.setUrl("/default/501.html"); //TODO hardcoded, debe obtener la ruta del config.
		std::string errorPath = _config.getErrorPage(501);
		_request.setUrl(errorPath); //TODO hardcoded, debe obtener la ruta del config.

		fd = readFile(socket, _request, _response);
		_response.setStatusCode("501");
		_response.setStatusMsg("METHOD_NOT_IMPLEMENTED");
		std::cerr << YELLOW << "Warning: Error 501 \"" << _request.getMethod() << "\", METHOD_NOT_IMPLEMENTED " << END << std::endl; */
	}
	if (fd > 2)
		close(fd);
}
