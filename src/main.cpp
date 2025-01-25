#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "SocketManager.hpp"

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Argument error: Invalid number of arguments" << std::endl;
		return(1);
	}

	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open()) {
		std::cerr << "Configuration error: cannot open config file" << std::endl;
		return (1);
	}

	Config::loadConfig(configFileStream);
	bool asServer = true;
	for (std::map<std::string, Server>::iterator it = Config::getServers().begin();
		it != Config::getServers().end(); ++it) {
		Socket	newSocket;
		newSocket.setPort((*it).second.getPort());
		newSocket.enableSocket(asServer);
		SocketManager::addSocket(newSocket);
	}
	SocketManager::monitorSockets();
	
	return 0;
}


