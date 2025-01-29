#include <iostream>
#include <csignal>
#include <cstdlib>
#include <signal.h>
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
 SocketManager::getSocketList().clear();//try with or without clear
 std::cerr << GREEN << "Signal " << signal << " received. Server shutting down." << END << std::endl;
 std::exit(EXIT_SUCCESS);

}
void SignalManager::signalSetUp() {
		signal(SIGINT, signalHandler);
		signal(SIGQUIT, signalHandler);
		signal(SIGTERM, signalHandler);
		signal(SIGTSTP, signalHandler);
}