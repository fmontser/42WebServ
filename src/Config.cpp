#include "Config.hpp"
#include <cstdlib>

#define MIN_PORT_NUMBER 0
#define MAX_PORT_NUMBER 65536
#define MIN_PAYLOAD 0
#define MAX_PAYLOAD 67108864 //64Mb

Config::Config() {}
Config::~Config() {}
Config::Config(const Config& src) {}
Config& Config::operator=(const Config& src) {}

/* void	Config::configureOptions() {
	_tokenMap["port"] = &Config::setPort;
	_tokenMap["maxPayload"] = &Config::setMaxPayload;
	_tokenMap["route"] = &Config::setRoute;
	_tokenMap["page404"] = &Config::setPage404;
} */

/* void	Config::configureRules() {
	_tokenMap["methods"] = NULL;
	_tokenMap["file"] = NULL;
} */



/* void	Config::tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList){
	char		c;
	std::string	token;
	bool		flag = false;
	
	token.clear();
	while (configFileStream.get(c)) {
		if (isspace(c)) {
			if (flag){
				flag = false;
				tokenList.push_back(token);
				token.clear();
			}
			else
				continue;
		}
		else {
			flag = true;
			token.append(std::string(1,c));
		}
	}
	tokenList.push_back(token);
	configFileStream.close();
} */

void	Config::loadConfig(std::fstream &configFileStream) {
	
/* 	std::vector<std::string> tokenList;
	std::vector<std::string>::iterator it;

	configureOptions();
	configureRules();
	tokenize(configFileStream, tokenList);

	for (it = tokenList.begin(); it != tokenList.end(); ++it){
		if (_tokenMap.find(*it) != _tokenMap.end())
			(this->*(_tokenMap[*it]))(it);
	}
	_webSocket = new WebSocket(_port); */

}




void	Config::setPort(std::vector<std::string>::iterator &it) {
	char	*err;
	int		portNumber;
	
	++it;
	portNumber = strtol((*it).c_str(), &err, 10);
	if (portNumber < MIN_PORT_NUMBER || portNumber > MAX_PORT_NUMBER || isalpha(*err)) {
		std::cerr << "Config file error: Invalid port number." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server Port set as: " << portNumber << std::endl;
	_port = portNumber;
}


void	Config::setMaxPayload(std::vector<std::string>::iterator &it) {
	char	*err;
	int		payloadSize;
	
	++it;
	payloadSize = strtol((*it).c_str(), &err, 10);
	if (payloadSize < MIN_PAYLOAD || payloadSize > MAX_PAYLOAD || isalpha(*err)) {
		std::cerr << "Config file error: Ivalid payload size." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Payload size set as: " << payloadSize << " bytes." << std::endl;
	_maxPayload = payloadSize;
}

void	Config::setRoute(std::vector<std::string>::iterator &it) {
	Route route;
	std::string key, value;

	route.value = *(++it);
	if (*(++it) == "{") {
		while(*(++it) != "}") {
			if (_tokenMap.find(*it) != _tokenMap.end()){
				key = *it;
				++it;
			}
			value = *it;
			route.rules.insert(std::make_pair(key, value));
		}
	}

	if (_routes.find(route.value) != _routes.end()) {
		std::cerr << "Config file error: Route " << route.value << " is duplicated." << std::endl;
		exit(1);
	}
	else
		_routes[route.value] = route;
}

void	Config::setPage404(std::vector<std::string>::iterator &it) {
	//TODO switch con todas las paginas
	++it;
	_page404 = *it;
}

__uint16_t								Config::getPort() const { return _port; }

__uint32_t								Config::getMaxPayload() const { return _maxPayload; }

std::map<std::string, Config::Route>&	Config::getRoutes() const { return (std::map<std::string, Config::Route>&)_routes; }

const std::string						Config::getPage404() const { return _page404; }

WebSocket&								Config::getWebSocket() const { return *_webSocket; }
