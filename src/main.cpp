#include <iostream>
#include <fstream>
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "SignalManager.hpp"
#include "Server.hpp"

int main(int argc, char** argv) {
	

	if (argc < 2) {
		std::cerr << "Argument error: Invalid number of arguments" << std::endl;
		return(1);
	}

	SignalManager::signalSetUp();
	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open()) {
		std::cerr << "Configuration error: cannot open config file" << std::endl;
		return (1);
	}

	Config::loadConfig(configFileStream);

	for (std::map<std::string, Server>::iterator it = Config::getServers().begin();
		it != Config::getServers().end(); ++it) {
			it->second.listenSocket();
	}
	
	ConnectionManager::monitorConnections();
	return 0;
}
