#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class FileManager {
private:
	HttpRequest		_request;
	HttpResponse	_response;

	FileManager(const FileManager& src);
	FileManager& operator=(const FileManager& src);

	void	processHttpRequest();
	bool	searchFile();
	bool	createFile();
	bool	deleteFile();
	bool	controlAccess();
public:
	FileManager();
	~FileManager();

	void	recieveHttpRequest(HttpRequest& request);
	void	recieveHttpResponse(HttpResponse& response);
};