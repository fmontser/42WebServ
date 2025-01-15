#include <iostream>
#include <fstream>
#include "Config.hpp"
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
	
	WebSocket ws(config.getPort());
	ws.start();
	WebServer server(ws);
	server.run();
	//TODO stuff
	

	return 0;
}
