#pragma once

#include <poll.h>

enum SocketType { LISTEN, CONNECTION };

class Socket {
	private:
		
		int				_port;
		struct pollfd	_pollFd;
		SocketType		_type;
		
	
	public:
		
		Socket(int port, struct pollfd pfd, SocketType type);
		~Socket();
		Socket(const Socket& src);
		Socket&	operator=(const Socket& src);
		bool	operator==(const Socket& src);
		bool	operator!=(const Socket& src);

		struct pollfd	getPollFd() const;
		SocketType		getType() const;
		int				getPort() const;

		void			setPollFd(struct pollfd pollFd);

		bool			hasPollErr() const;
		bool			hasPollIn() const;
		bool			hasPollOut() const;
};