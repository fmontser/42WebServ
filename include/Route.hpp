#pragma once

#include <map>
#include <string>

class Route {
	private:
		std::string								_url;
		std::string								_default;
		std::multimap<std::string, std::string>	_methods;
		std::string								_autoIndex;
		std::string								_root;
		std::string								_redirect;
		std::string								_upload;

	public:
		Route();
		~Route();
		Route(const Route& src);
		Route& operator=(const Route& src);

		std::string								getUrl() const;
		std::string								getAutoIndex() const;
		std::string								getRoot() const;
		std::string								getRedirect() const;
		std::multimap<std::string, std::string>	getMethods() const;
		std::string								getDefault() const;
		std::string								getUpload() const;

		void	setUrl(const std::string& url);
		void	addMethod(const std::pair<std::string, std::string> method);
		void	setAutoIndex(const std::string autoIndex);
		void	setRoot(const std::string root);
		void	setRedirect(const std::string redirect);
		void	setDefault(const std::string default_);
		void	setUpload(const std::string upload);
		bool	isMethodAllowed(std::string method);
};
