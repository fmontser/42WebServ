#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include "Config.hpp"
#include "FileManager.hpp"
#include "DataAdapter.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "Utils.hpp"

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

static void chunkEncode(std::vector<char>& body, size_t maxPayload) {
	std::stringstream	buffer;
	std::vector<char>	_body(body);
	int					byte;

	while (_body.size()) {
		int	csize = _body.size() >= maxPayload ? maxPayload : _body.size();

		buffer << std::hex << csize << CRLF;
		buffer << std::string(_body.begin(), _body.begin() + csize);
		_body.erase(_body.begin(), _body.begin() + csize);
		buffer << CRLF;
	}
	buffer << '0' << CRLF << CRLF;
	body.clear();
	while ((byte = buffer.get()) != EOF)
		body.push_back(byte);
}

HttpResponse::responseType	FileManager::readFile(DataAdapter& dataAdapter) {
	std::string			target; 
	int					fd, readSize, i;
	Server&				server = dataAdapter.getConnection()->getServer();
	HttpRequest&		request = dataAdapter.getRequest();
	HttpResponse&		response = dataAdapter.getResponse();
	char				readBuffer[READ_BUFFER] = {0};
	
	target = server.getRoot().substr(0, server.getRoot().size() - 1).append(request.url);
	if (isDirectory(request.url))
		target.append(server.getRoutes().find(request.url)->second.getDefault());
	
	if(access(target.c_str(), F_OK != 0))
		return HttpResponse::NOT_FOUND;
	
	if (access(target.c_str(), R_OK) != 0)
		return HttpResponse::FORBIDDEN;

	fd = open(target.c_str(), O_RDONLY, 0644);
	if (fd < 0)
		return HttpResponse::SERVER_ERROR;

	do {
		i = 0;
		readSize = read(fd, readBuffer, READ_BUFFER);
		while (i < readSize)
		response.body.push_back(readBuffer[i++]);
	} while (readSize);

	if ((int)response.body.size() > server.getMaxPayload()) {
		response.addHeader("Transfer-Encoding: chunked");
		chunkEncode(response.body, server.getMaxPayload());
	}
	else {
		std::stringstream	contentLengthHeader;
		contentLengthHeader << "Content-Length: " << response.body.size();
		response.addHeader(contentLengthHeader.str());
	}
	close(fd);
	return HttpResponse::OK;
}

#include <algorithm>

HttpResponse::responseType	FileManager::writeFile(DataAdapter& dataAdapter) {
	HttpRequest&	request = dataAdapter.getRequest();
	std::string		fileName, uploadDir;
	int				fd;

	uploadDir.append(dataAdapter.getConnection()->getServer().getRoot());
	uploadDir.append(dataAdapter.getConnection()->getServer().getUploadDir());
	if (access(uploadDir.c_str(), F_OK) != 0)
		mkdir(uploadDir.c_str(), 0777);

	for (std::vector<HttpHeader>::iterator it = dataAdapter.getRequest().headers.begin();
			it != dataAdapter.getRequest().headers.end(); ++it) {

		if (it->name == "Content-Disposition"){
			HeaderValue value;
			it->getValue("Content-Disposition", &value);
			if (!value.properties.empty()) {
				HeaderProperty propertie;
				value.getPropertie("filename", &propertie);
				if (!propertie.value.empty()){
					std::string	cropped = propertie.value;
					Utils::nestedQuoteExtract('"', cropped);
					fileName.append(uploadDir);
					fileName.append(cropped);
					break;
				}
			}
		}
	}

	if (access(uploadDir.c_str(), W_OK) != 0)
		return HttpResponse::FORBIDDEN;
	if ((access(fileName.c_str(), F_OK) == 0 && dataAdapter.allowFileAppend)
		|| access(fileName.c_str(), F_OK) != 0) {

		fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0775);
		if (fd > 0) {
			write(fd, &request.body[0], request.body.size());
			close(fd);
		}
		else 
			return HttpResponse::SERVER_ERROR;
	}
	else 
		return HttpResponse::CONFLICT;

	if (dataAdapter.getConnection()->requestMode == Connection::MULTIPART)
		dataAdapter.allowFileAppend = true;
	return HttpResponse::CREATED;
}