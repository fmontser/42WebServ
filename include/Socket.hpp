#pragma once
#include <poll.h>
#include <string>

class Server;

class Socket {
	private:
		unsigned int	_port;
		int				_fd;
		bool			_serverFlag;
		bool			_chunkMode;
		Server			*_parentServer;
		struct pollfd	_pollfd;

	public:
		std::string		sendBuffer;

		Socket();
		~Socket();
		Socket(const Socket& src);
		Socket& operator=(const Socket& src);
		bool operator==(const Socket& other);

		void					enableSocket(bool serverFlag);

		unsigned int			getPort() const;
		int						getFd() const;
		const struct pollfd&	getPollFd() const;
		bool					getServerFlag() const;
		bool					getChunkMode() const;
		Server					*getParentServer() const;
		
		void					setPort(unsigned int port);
		void					setFd(int fd);
		void					setParentServer(Server *parentServer);
		void					setChunkMode(bool value);
		void					updatePollFd(struct pollfd pfd);
		
};
