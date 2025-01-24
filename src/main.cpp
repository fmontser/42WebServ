#include <iostream>
#include <fstream>
#include "WebServer.hpp"

extern WebServer webServer;

int main(int argc, char** argv) {
	if (argc < 2)
		return(1);

	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open())
		return (1);

	webServer.config = Config();
	webServer.socketManager = SocketManager();
	webServer.dataAdapter = DataAdapter();

	return 0;
}


