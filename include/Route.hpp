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

		void	setUrl(const std::string& url);
		void	addMethod(const std::pair<std::string, std::string>& method);
		void	addFile(const std::pair<std::string, std::string>& file);
};
