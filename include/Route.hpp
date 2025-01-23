#pragma once

#include <map>
#include <string>

class Route {
	private:
		std::string							_url;
		std::map<std::string, std::string>	_methods;
		std::map<std::string, std::string>	_files;

	public:
		Route();
		~Route();
		Route(const Route& src);
		Route& operator=(const Route& src);

		std::string							getUrl() const;
		std::map<std::string, std::string>	getMethods() const;
		std::map<std::string, std::string>	getFiles() const;

		void								setUrl(const std::string& url);
		void								setMethods(std::map<std::string, std::string>& methods);
		void								setFiles(std::map<std::string, std::string>& files);
};
