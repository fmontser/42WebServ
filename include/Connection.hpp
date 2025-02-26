#pragma once

#include <poll.h>
#include <string>
#include "ServerConstants.hpp"
#include "Server.hpp"
#include "DataAdapter.hpp"

class Connection {
	private:

		Server&			_server;
		int				_socketFd;
		struct pollfd	_pollfd;
		DataAdapter		*_dataAdapter;

	public:

		std::string	recvBuffer;
		std::string	sendBuffer;
		bool		isChunkedResponse;
		bool		isMultipartUpload;
		std::string	boundarie;
		size_t		contentLength;

		Connection(Server& server);
		Connection(const Connection& src);
		Connection& operator=(const Connection& src);
		~Connection();

		Server&			getServer() const;
		struct pollfd	getPollFd() const;

		void			recieveData();
		void			sendData();
		void			updatePollFd(struct pollfd pfd);
		bool			hasPollIn() const;
		bool			hasPollOut() const;
};