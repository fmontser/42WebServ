#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include "Config.hpp"
#include "FileManager.hpp"
#include "DataAdapter.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"

FileManager::FileManager() {}
FileManager::~FileManager() {}

FileManager::FileManager(const FileManager& src) { (void)src; }

FileManager& FileManager::operator=(const FileManager& src) {
	if (this != &src) {
		(void)src;
	}
	return *this;
}

static bool isDirectory(const std::string& url) {
	return (url.substr(url.size() - 1, 1) == "/") ? true : false;
}

static void chunkEncode(std::string& body, size_t maxPayload) {
	std::stringstream	buffer;

	while (body.size()) {
		int	csize = body.size() >= maxPayload ? maxPayload : body.size();
		buffer << std::hex << csize << CRLF;
		buffer << body.substr(0, csize) << CRLF;
		body = body.substr(csize, body.size());
	}
	buffer << '0' << CRLF << CRLF;
	body = buffer.str();
}

void	FileManager::readFile(DataAdapter& dataAdapter) {
	std::string			target, body; 
	int					fd, readSize;
	Server&				server = dataAdapter.getConnection()->getServer();
	HttpRequest&		request = dataAdapter.getRequest();
	HttpResponse&		response = dataAdapter.getResponse();
	
	target = server.getRoot().substr(0, server.getRoot().size() - 1).append(request.url);
	if (isDirectory(request.url))
		target.append("index.html"); //TODO hardcoded, obtener de config
	
	fd = open(target.c_str(), O_RDONLY, 0644);
	if (fd < 0) {
		fd = open("../web/default/404.html", O_RDONLY, 0644); //TODO hardcoded, debe obtener la ruta del config.
		response.statusCode = "404";
		response.statusMsg = "NOT_FOUND";
		std::cerr << YELLOW << "Warning: error 404 \"" << target << "\", NOT_FOUND " << END << std::endl;
	}
	do {
		char readBuffer[READ_BUFFER] = {0};
		readSize = read(fd, readBuffer, READ_BUFFER);
		body.append(readBuffer);
	} while (readSize > 0);
	
	if ((int)body.size() > server.getMaxPayload()){
		response.addHeader("Transfer-Encoding: chunked");
		chunkEncode(body, server.getMaxPayload());
	}
	else {
		std::stringstream	contentLengthHeader;
		contentLengthHeader << "Content-Length: " << body.size();
		response.addHeader(contentLengthHeader.str());

	}
	response.body = body;
	close(fd);
}

void	FileManager::writeFile(DataAdapter& dataAdapter) {
	//TODO implementar
	(void)dataAdapter;
}