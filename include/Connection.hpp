#pragma once

#include <poll.h>
#include <string>
#include "ServerConstants.hpp"
#include "Server.hpp"

class Connection {
	private:

		Server&			_server;
		int				_socketFd;
		struct pollfd	_pollfd;

		Connection& operator=(const Connection& src);

		void	acceptConnection();

	public:

		std::string	recvBuffer;
		std::string	sendBuffer;
		int			sendBufferSize;

		Connection(Server& server);
		Connection(const Connection& src);
		~Connection();

		Server&			getServer() const;
		struct pollfd	getPollFd() const;
		bool			hasPollIn() const;
		bool			hasPollOut() const;

		void			recieveData();
		void			sendData();
		void			updatePollFd(struct pollfd pfd);
};