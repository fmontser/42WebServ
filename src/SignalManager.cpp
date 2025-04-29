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

	for (std::list<Socket>::iterator socket = ConnectionManager::_socketList.begin()
		; socket != ConnectionManager::_socketList.end(); ++socket) {
			close(socket->getPollFd().fd);
	}

	for (std::list<Connection *>::iterator connection = ConnectionManager::_connectionList.begin()
			; connection != ConnectionManager::_connectionList.end(); ++connection) {
		if (*connection)
				delete *connection;
	}
	
	std::cout << std::endl << BLUE << "Signal " << signal << " received. Server shutting down." << END << std::endl;
	std::exit(EXIT_SUCCESS);
}

void SignalManager::signalSetUp() {
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGTSTP, signalHandler);
}