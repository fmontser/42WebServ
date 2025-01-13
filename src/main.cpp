#include <iostream>
#include <fstream>
#include "Config.hpp"

int main(int argc, char** argv) {

	std::string	line;

	if (argc < 2)
		return(1);

	std::fstream configFileStream(argv[1]);
	if (!configFileStream.is_open())
		return (1);

	Config config(configFileStream);

	//TODO stuff
	
	configFileStream.close();
	return 0;
}
