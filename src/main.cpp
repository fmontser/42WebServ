#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "HttpReq.hpp"

int main(int argc, char** argv) {

	std::string	line;

	if (argc < 2)
		return(1);

	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open())
		return (1);

	Config config(configFileStream);

	std::stringstream httpRequest;


	httpRequest << "GET /recurso HTTP/1.1\r\n"
				<< "Host: www.42.com\r\n"
				<< "Accept: text/html, application/xhtml+xml, application/xml;q=0.9\r\n"
				<< "User-Agent: MiNavegador/1.0\r\n"
				<< "Content-Length: 13\r\n"
				<< "\r\n"
				<< "Hola mundo!\n"
				<< "Hola mundo!\n"
				<< "Hola mundo!\n"
				<< "Hola mundo!\n";

	HttpReq test(&httpRequest);
	return 0;
}
