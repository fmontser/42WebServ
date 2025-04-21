#include <iostream>
#include <csignal>
#include <cstdlib>
#include <signal.h>
#include "Config.hpp"
#include "SignalManager.hpp"
#include "ServerConstants.hpp"
#include "ConnectionManager.hpp"

SignalManager::SignalManager() {}
SignalManager::~SignalManager() {}
SignalManager::SignalManager(const SignalManager& src) {	
	(void)src;
}

SignalManager& SignalManager::operator=(const SignalManager& src) {
	(void)src;
	return *this;
}

void SignalManager::signalHandler(int signal) {
	//TODO (hacer esto lo ultimo!!!) liberar socketFD y los servers y cualquier otra cosa 
	std::cout << std::endl << BLUE << "Signal " << signal << " received. Server shutting down." << END << std::endl;
	std::exit(EXIT_SUCCESS);
}

void SignalManager::signalSetUp() {
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGTSTP, signalHandler);
}