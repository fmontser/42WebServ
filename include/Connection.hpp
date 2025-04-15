#pragma once

#include <poll.h>
#include <string>
#include "ServerConstants.hpp"
#include "Server.hpp"
#include "DataAdapter.hpp"
#include "CgiAdapter.hpp"

class Connection {
	private:

		Server&			_server;
		int				_socketFd;
		struct pollfd	_pollfd;
		DataAdapter		*_multiDataAdapter;
		CgiAdapter		*_multiCgiAdapter;

	public:

		enum RequestMode { SINGLE, MULTIPART };
		//enum ResponseMode { SINGLE, CHUNKED };

		std::vector<char>	recvBuffer;
		std::vector<char>	sendBuffer;
		bool				isChunkedResponse;	//TODO cambiar a response mode (enums)
		bool				isOverPayloadLimit;
		RequestMode			requestMode;
		std::string			boundarie;
		std::string			boundStart;
		std::string			boundEnd;
		size_t				contentLength;

		Connection(Server& server);
		Connection(const Connection& src);
		Connection& operator=(const Connection& src);
		~Connection();

		Server&			getServer() const;
		struct pollfd	getPollFd() const;

		void			recieveData();
		void			sendData();
		void			updatePollFd(struct pollfd pfd);
		bool			hasPollErr() const;
		bool			hasPollIn() const;
		bool			hasPollOut() const;
};