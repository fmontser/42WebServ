#pragma once

class Socket {
	private:
		unsigned int	_port;
		int				_fd;
		
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);
	public:
		Socket();
		~Socket();

		void	configureSocket();

		int				getFd() const;
		unsigned int	getPort() const;

		void			setPort(unsigned int port);
};
