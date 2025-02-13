#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Socket.hpp"
#include "Server.hpp"

class FileManager {
	private:
		static HttpRequest	_request;
		static HttpResponse	_response;

		FileManager();
		~FileManager();
		FileManager(const FileManager& src);
		FileManager& operator=(const FileManager& src);

		static void	processHttpRequest(Socket *targetSocket);
	public:
		static void processMultiPart(Socket * socket);
		static void	recieveHttpRequest(Socket *targetSocket, HttpRequest& request);
		static void	recieveHttpResponse(Socket *targetSocket, HttpResponse& response);
};