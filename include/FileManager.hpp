#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class FileManager {
	private:
		static HttpRequest	_request;
		static HttpResponse	_response;

		FileManager();
		~FileManager();
		FileManager(const FileManager& src);
		FileManager& operator=(const FileManager& src);

		static void	processHttpRequest();
		static bool	searchFile();
		static bool	createFile();
		static bool	deleteFile();
		static bool	controlAccess();
	public:
		static void	recieveHttpRequest(HttpRequest& request);
		static void	recieveHttpResponse(HttpResponse& response);
};