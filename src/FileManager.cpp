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

//TODO comprobar permisos de lectura (403 Forbidden), devolver 403?
//TODO si no existe (404 Not found), devolver 404?
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
		body.append(readBuffer); //TODO &&&& sustituir por char vector
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
	response.body.assign(body.c_str(), body.c_str() + body.size());
	close(fd);
}

#include <algorithm>

int	FileManager::writeFile(DataAdapter& dataAdapter) {
	HttpRequest&	request = dataAdapter.getRequest();
	std::string		fileName, uploadDir;
	int				fd;

	uploadDir.append(dataAdapter.getConnection()->getServer().getRoot());
	uploadDir.append(dataAdapter.getConnection()->getServer().getUploadDir());
	if (!access(uploadDir.c_str(), F_OK) == 0)
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

	if (!access(uploadDir.c_str(), W_OK) == 0) {
		return 403; //TODO si no hay permisos 403 Forbidden
	}

	if ((access(fileName.c_str(), F_OK) == 0 && dataAdapter.allowFileAppend)
		|| !access(fileName.c_str(), F_OK) == 0) {

		fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0775);
		if (fd > 0) {
			write(fd, &request.body[0], request.body.size());
			close(fd);
		}
		else {
			std::cerr << "Error: Server error" << std::endl;
			return 500;
		}
	}
	else 
		return 409; //TODO si ya existe 409 Conflict

	if (dataAdapter.getConnection()->requestMode == Connection::MULTIPART)
		dataAdapter.allowFileAppend = true;
	return 200;
}