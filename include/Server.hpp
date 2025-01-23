#pragma once

#include <map>
#include <string>

class Server {
	private:
		std::string	_name;
		std::string	_host;
		int			_port;
	public:
		Server();
		~Server();
		Server(const Server& src);
		Server& operator=(const Server& src);

	std::string	getName() const;
	std::string	getHost() const;
	int			getPort() const;

	void	setName(const std::string& name);
	void	setHost(const std::string& host);
	void	setPort(int port);
};
