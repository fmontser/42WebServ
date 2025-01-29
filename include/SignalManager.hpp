#pragma once

class SignalManager
{
	public:
	SignalManager();
	~SignalManager();
	SignalManager(const SignalManager& src);
	SignalManager& operator=(const SignalManager& src);


	static	void signalHandler(int signal);
	static	void signalSetUp();
};


