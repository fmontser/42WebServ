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

		void	manageSingle(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter);
		void	manageMultiPart(DataAdapter& dataAdapter, CgiAdapter& cgiAdapter);
		void	resetConnection();

	public:

		enum RequestMode { SINGLE, MULTIPART };
		enum ResponseMode { NORMAL, CHUNKED };

		std::vector<char>	recvBuffer;
		std::vector<char>	sendBuffer;
		bool				isOverPayloadLimit;
		bool				hasPendingCgi;
		RequestMode			requestMode;
		ResponseMode		responseMode;
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

		void			fetchCgi();
		void			recieveData();
		void			sendData();
		void			updatePollFd(struct pollfd pfd);
		bool			hasPollErr() const;
		bool			hasPollIn() const;
		bool			hasPollOut() const;
};