#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <algorithm>
#include "Config.hpp"
#include "FileManager.hpp"
#include "DataAdapter.hpp"
#include "ServerConstants.hpp"
#include "Connection.hpp"
#include "Utils.hpp"
#include "PathManager.hpp"


FileManager::FileManager() {}
FileManager::~FileManager() {}

FileManager::FileManager(const FileManager& src) { (void)src; }

FileManager& FileManager::operator=(const FileManager& src) {
	if (this != &src) {
		(void)src;
	}
	return *this;
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

static void setDownloadResponse(DataAdapter& dataAdapter, std::string path) {
	std::string contentType, fileName;

	fileName = Utils::getFileName(path);

	HttpHeader contentDispHeader;
	contentDispHeader.name = "Content-Disposition";
	HeaderValue cdValue;
	cdValue.name = "attachment; filename=\"" + fileName + "\"";
	contentDispHeader.addValue(cdValue);
	dataAdapter.getResponse().addHeader(contentDispHeader);
}


HttpResponse::responseType	FileManager::readFile(DataAdapter& dataAdapter) {
	std::string			path;
	int							fd, readSize, i;
	Server&					server = dataAdapter.getConnection()->getServer();
	HttpRequest&		request = dataAdapter.getRequest();
	HttpResponse&		response = dataAdapter.getResponse();
	char						readBuffer[READ_BUFFER] = {0};

	if (!response.statusCode.empty())
		path.append(request.url);
	else
		path = PathManager::resolveRoutePath(dataAdapter);
	
	if(access(path.c_str(), F_OK != 0))
		return HttpResponse::NOT_FOUND;
	
	if (access(path.c_str(), R_OK) != 0)
		return HttpResponse::FORBIDDEN;

	fd = open(path.c_str(), O_RDONLY, 0644);
	if (fd < 0)
		return HttpResponse::SERVER_ERROR;
	do {
		i = 0;
		readSize = read(fd, readBuffer, READ_BUFFER);
		while (i < readSize)
			response.body.push_back(readBuffer[i++]);
	} while (readSize);

	if (request.isBinaryDownload)
		setDownloadResponse(dataAdapter, path);

	if (response.body.size() > server.getMaxPayload()) {
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



HttpResponse::responseType	FileManager::writeFile(DataAdapter& dataAdapter) {
	HttpRequest&	request = dataAdapter.getRequest();
	std::string		fileName, path;
	int				fd;

	path = PathManager::resolveRoutePath(dataAdapter);

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
					PathManager::stackPath(fileName, path);
					PathManager::stackPath(fileName, cropped);
					break;
				}
			}
		}
	}

	if (access(path.c_str(), W_OK) != 0)
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

HttpResponse::responseType	FileManager::deleteFile(DataAdapter& dataAdapter) {
	std::string	path;

	path = PathManager::resolveRoutePath(dataAdapter);

	if (access(path.c_str(), F_OK) != 0)
		return HttpResponse::NOT_FOUND;
	if (access(path.c_str(), W_OK) != 0)
		return HttpResponse::FORBIDDEN;
	if (remove(path.c_str()) != 0)
		return HttpResponse::SERVER_ERROR;
	return HttpResponse::NO_CONTENT;
}

HttpResponse::responseType FileManager::downloadFile(DataAdapter& dataAdapter) {
		HttpRequest& request = dataAdapter.getRequest();
		HttpResponse& response = dataAdapter.getResponse();

		std::string filePath = ".." + dataAdapter.getConnection()->getServer().getRoot() + request.url;
		std::string fileName = request.url.substr(request.url.find_last_of('/') + 1);

		if (access(filePath.c_str(), F_OK) == -1) {
		}

		std::ifstream file(filePath.c_str(), std::ios::binary);
		if (!file.is_open()) {
			return HttpResponse::SERVER_ERROR;
		}


		file.seekg(0, std::ios::end);
		size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::string contentType;


		std::string ext = Utils::getFileType(fileName);

		if (ext == "html" || ext == "htm") {
			contentType = "text/html";
		} else if (ext == "css") {
			contentType = "text/css";
		} else if (ext == "js") {
			contentType = "application/javascript";
		} else if (ext == "png") {
			contentType = "image/png";
		} else if (ext == "jpg" || ext == "jpeg") {
			contentType = "image/jpeg";
		} else if (ext == "gif") {
			contentType = "image/gif";
		} else if (ext == "svg") {
			contentType = "image/svg+xml";
		} else if (ext == "txt") {
			contentType = "text/plain";
		} else if (ext == "pdf") {
			contentType = "application/pdf";
		} else if (ext == "zip") {
			contentType = "application/zip";
		} else if (ext == "json") {
			contentType = "application/json";
		} else if (ext == "xml") {
			contentType = "application/xml";
		} else {
			contentType = "application/octet-stream";
		}

		HttpHeader contentTypeHeader;
		contentTypeHeader.name = "Content-Type";
		HeaderValue ctValue;
		ctValue.name = contentType;
		contentTypeHeader.addValue(ctValue);
		response.addHeader(contentTypeHeader);

		HttpHeader contentDispHeader;
		contentDispHeader.name = "Content-Disposition";
		HeaderValue cdValue;
		cdValue.name = "attachment; filename=\"" + fileName + "\"";
		contentDispHeader.addValue(cdValue);
		response.addHeader(contentDispHeader);

		HttpHeader contentLengthHeader;
		contentLengthHeader.name = "Content-Length";
		HeaderValue clValue;
		clValue.name = Utils::toString(fileSize);
		contentLengthHeader.addValue(clValue);
		response.addHeader(contentLengthHeader);
		


		std::vector<char> buffer(fileSize);
		file.read(buffer.data(), fileSize);
		if (!file) {
			file.close();
			return HttpResponse::SERVER_ERROR;
		}


		response.body = buffer;
		file.close();
		return HttpResponse::OK;

}