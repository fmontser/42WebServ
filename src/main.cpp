#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "WebSocket.hpp"
#include "WebServer.hpp"


int main(int argc, char** argv) {

	std::string	line;

	if (argc < 2)
		return(1);

	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open())
		return (1);

	Config config(configFileStream);
	WebServer server(config);

	return 0;
}

<<Processor>>
CgiProcessor
--
- response : HttpResponse
--
+ CgiProcessor() 
+ ~CgiProcessor()
- CgiProcessor(const CgiProcessor& src)
- operator=(const CgiProcessor& src) : const CgiProcessor&

+ recieveHttpResponse(HttpResponse& Response) : void
- processHttpResponse() : void
- setEnviorment() : void
- executeCGI();
--
Input: FileManager -> HttpResponse
Output: HttpResponse -> FileManager

Manage cgi response modification

