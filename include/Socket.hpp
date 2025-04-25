#pragma once

#include <poll.h>

enum SocketType { LISTEN, CONNECTION };

class Socket {
	private:
		
		struct pollfd	_pollFd;
		SocketType		_type;
		
	
	public:
		
		Socket(struct pollfd pfd, SocketType type);
		~Socket();
		Socket(const Socket& src);
		Socket&	operator=(const Socket& src);
		bool	operator==(const Socket& src);
		bool	operator!=(const Socket& src);

		struct pollfd	getPollFd() const;
		SocketType		getType() const;

		void			setPollFd(struct pollfd pollFd);

		bool			hasPollErr() const;
		bool			hasPollIn() const;
		bool			hasPollOut() const;
};