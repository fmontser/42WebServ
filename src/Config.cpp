#include "Config.hpp"



Config::Config(std::fstream &configFileStream) {
	std::string line;

	tokenMap["port"] = &Config::setPort;
	tokenMap["maxPayload"] = &Config::setMaxPayload;
	tokenMap["route"] = &Config::setRoute;
	tokenMap["page404"] = &Config::setPage404;


	while (std::getline(configFileStream, line)) {
		line = line.substr(0, line.find('#'));

		std::string key = line.substr(0, line.find(' '));
		std::string value = line.substr(key.size() + 1, line.size());
		value = value.substr(0, value.find(' '));

		//TODO @@@@@@ quitar whitespaces de key y values
		//TODO manejar keys no existentes!

		(this->*(tokenMap[key]))(value);
	}
}

Config::~Config() {}

void	Config::setPort(std::string value) {
	std::cout << "Port is " << value << std::endl;
}
void	Config::setMaxPayload(std::string value) {
	std::cout << "Payload is " << value << std::endl;
}
void	Config::setRoute(std::string value) {
	std::cout << "Route is " << value << std::endl;
}
void	Config::setPage404(std::string value) {
	std::cout << "Page404 is " << value << std::endl;
}

void Config::parseValue() {

}

void Config::parseRoute() {

}

void Config::parseRule() {

}

