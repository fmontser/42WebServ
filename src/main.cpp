#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

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

	HttpRequest test;

	test.pull(&httpRequest);

	HttpResponse responseTest;

	responseTest.setVersion("HTTP/1.1");
	responseTest.setStatusCode("200");
	responseTest.setStatusMsg("OK");
	responseTest.addHeader("Server", "Webserv42");
	responseTest.addHeader("Date", "Date test");
	responseTest.addHeader("Cacafruti", "all over the place");
	responseTest.setBody("Body content test blah blah");

	responseTest.push();
	
	return 0;
}
