#pragma once

#include <map>
#include <string>

class Route {
	private:
		std::string								_url;
		std::string								_default;

		std::multimap<std::string, std::string>	_methods;
		std::multimap<std::string, std::string>	_files;
		std::pair<std::string, std::string>		_autoIndex;
		std::pair<std::string, std::string>		_root;
		std::pair<std::string, std::string>		_redirect;

	public:
		Route();
		~Route();
		Route(const Route& src);
		Route& operator=(const Route& src);

		std::string							getUrl() const;
		std::multimap<std::string, std::string>	getMethods() const;
		std::multimap<std::string, std::string>	getFiles() const;

		std::string								getDefault() const;

		void	setUrl(const std::string& url);
		void	addMethod(const std::pair<std::string, std::string> method);
		void	addFile(const std::pair<std::string, std::string> file);
		void	setAutoIndex(const std::pair<std::string, std::string> autoIndex);
		void	setRoot(const std::pair<std::string, std::string> root);
		void	setRedirect(const std::pair<std::string, std::string> redirect);
		void	setDefault(const std::string& default_);
};
