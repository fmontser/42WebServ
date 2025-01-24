#pragma once

class Socket {
	private:
		unsigned int		_port;
		int					_fd;
		struct sockaddr_in 	_serv_addr;
		
	public:
		Socket(int port);
		~Socket();
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);

		void			configureSocket();
		void			start();

		int				getFd() const;
		unsigned int	getPort() const;

		void			setPort(unsigned int port);
};
