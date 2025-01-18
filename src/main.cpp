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
	WebSocket webSocket(config.getPort());
	WebServer server(webSocket);
	
	return 0;
}