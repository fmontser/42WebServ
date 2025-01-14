#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

class Config {
	public:
		struct Route
		{
			std::string								value;
			std::multimap<std::string, std::string>	rules;
			Route();
			~Route();
		};

	private:
		std::map<std::string, void (Config::*)(std::vector<std::string>::iterator &it)>	tokenMap;
		__uint16_t						port;
		__uint32_t						maxPayload;
		std::map<std::string, Route>	routes;
		std::string						page404;

		void	setPort(std::vector<std::string>::iterator &it);
		void	setMaxPayload(std::vector<std::string>::iterator &it);
		void	setRoute(std::vector<std::string>::iterator &it);
		void	setPage404(std::vector<std::string>::iterator &it);

		void	tokenize(std::fstream &configFileStream, std::vector<std::string> &tokenList);
		void	configureOptions();
		void	configureRules();

	public:

		Config(std::fstream &configFileStream);
		~Config();

		__uint16_t					getPort() const;
		__uint32_t					getMaxPayload() const;
		std::map<std::string, int>	getRoutes() const;
		const std::string			getPage404() const;
};

