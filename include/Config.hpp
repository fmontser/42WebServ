#pragma once
#include <iostream>
#include <map>
#include <fstream>

/* struct Route
{
	std::string	value;
	std::map<std::string, std::string>rules;
	Route();
	~Route();
}; */

class Config {
	private:
		std::map<std::string, void (Config::*)(std::string)>	tokenMap;
		__uint16_t							port;
		__uint32_t							maxPayload;
		std::map<std::string, int>		routes;
		std::string							page404;
	public:
		Config(std::fstream &configFileStream);
		~Config();

		void	setPort(std::string value);
		void	setMaxPayload(std::string value);
		void	setRoute(std::string value);
		void	setPage404(std::string value);

		void parseValue();
		void parseRoute();
		void parseRule();
};