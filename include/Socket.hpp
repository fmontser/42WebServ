#pragma once
#include <poll.h>

class Socket {
	private:
		unsigned int	_port;
		int				_fd;
		bool			_serverFlag;
		struct pollfd	_pollfd;

	public:
		Socket();
		~Socket();
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);
		bool operator==(const Socket& other);

		void					enableSocket(bool serverFlag);

		unsigned int			getPort() const;
		int						getFd() const;
		const struct pollfd&			getPollFd() const;
		bool					getServerFlag() const;
		
		void					setPort(unsigned int port);
		void					setFd(int fd);
		void					updatePollFd(struct pollfd pfd);
};
