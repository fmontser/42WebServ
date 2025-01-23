#pragma once

class Socket {
	private:
		unsigned int	_port;
		int				_fd;
	public:
		Socket();
		~Socket();
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);

		void	configureSocket();

		int				getFd() const;
		unsigned int	getPort() const;

		void			setPort(unsigned int port);
};
