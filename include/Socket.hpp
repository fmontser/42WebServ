#pragma once
#include <poll.h>
#include <string>

class Server;

class Socket {
	private:
		unsigned int	_port;
		int				_fd;

		Server			*_parentServer;
		struct pollfd	_pollfd;

	public:
		std::string	sendBuffer;
		std::string recvBuffer;
		std::string	boundarie;
		int			contentLength;
		bool		serverMode;
		bool		chunkMode;
		bool		multiMode;

		Socket();
		~Socket();
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);
		bool operator==(const Socket& other);

		void					enableSocket(bool serverMode);

		unsigned int			getPort() const;
		int						getFd() const;
		const struct pollfd&	getPollFd() const;
		Server					*getParentServer() const;
		
		void					setPort(unsigned int port);
		void					setFd(int fd);
		void					setParentServer(Server *parentServer);
		void					updatePollFd(struct pollfd pfd);
		
};
