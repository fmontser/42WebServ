#include <iostream>
#include <csignal>
#include <cstdlib>
#include <signal.h>
#include "Config.hpp"
#include "SignalManager.hpp"
#include "ServerConstants.hpp"
#include "SocketManager.hpp"

SignalManager::SignalManager() {}
SignalManager::~SignalManager() {}
SignalManager::SignalManager(const SignalManager& src) {	
	(void)src;
}

SignalManager& SignalManager::operator=(const SignalManager& src) {
	(void)src;
	return *this;
}

void SignalManager::signalHandler(int signal)
{
for (std::map<std::string, Server>::iterator servIt = Config::getServers().begin();
			servIt != Config::getServers().end(); ++servIt) {
	for (std::list<Socket *>::iterator it = servIt->second.getSocketList().begin();
			it != servIt->second.getSocketList().end(); ++it) {
				delete (*it);
			}
	servIt->second.getSocketList().clear();
	std::cerr << BLUE << "Signal " << signal << " received. Server shutting down." << END << std::endl;
}
 std::exit(EXIT_SUCCESS);

}
void SignalManager::signalSetUp() {
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGTSTP, signalHandler);
}