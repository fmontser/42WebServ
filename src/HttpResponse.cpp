#include <iostream>
#include <sstream>
#include "DataAdapter.hpp"
#include "HttpResponse.hpp"
#include "FileManager.hpp"
#include "PathManager.hpp"
#include "Index.hpp"
#include "ServerConstants.hpp"
#include "Utils.hpp"

HttpResponse::HttpResponse() : HttpMessage() {}

HttpResponse::HttpResponse(const HttpResponse& src) : HttpMessage() {
	this->statusCode = src.statusCode;
	this->statusMsg = src.statusMsg;
}
HttpResponse& HttpResponse::operator=(const HttpResponse& src) {
	if (this != &src) {
		this->statusCode = src.statusCode;
		this->statusMsg = src.statusMsg;
	}
	return *this;
}

HttpResponse::~HttpResponse() {}

int	HttpResponse::isChunked() {
	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		HttpHeader header = *it;
		HeaderValue value;

		if (header.getValue("Transfer-Encoding", &value) && value.name == "chunked")
			return Connection::CHUNKED;
	}
	return Connection::NORMAL;
}

void	HttpResponse::setupResponse(enum responseType responseType, DataAdapter& dataAdapter) {
	std::string contentLength ("Content-Length: ");

	version = HTTP_VERSION;
	switch (responseType) {
		case DIR_LIST:
			statusCode = "200";
			statusMsg = "OK";
			body = Index::generateAutoindex(dataAdapter);
			addHeader(contentLength.append(Utils::getStringSizeStr(body.size())));
			std::cout << "LISTING > ";
			break;
		case CONTINUE:
			statusCode = "100";
			statusMsg = "CONTINUE";
			break;
		case OK:
			statusCode = "200";
			statusMsg = "OK";
			addHeader(contentLength.append(Utils::getStringSizeStr(body.size())));
			break;
		case CREATED:
			statusCode = "201";
			statusMsg = "CREATED";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default201");
			FileManager::readFile(dataAdapter);
			break;
		case NO_CONTENT:
			statusCode = "204";
			statusMsg = "NO_CONTENT";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default204");
			break;
		case SEE_OTHER:
 			statusCode = "303";
			statusMsg = "SEE_OTHER";
			addHeader(PathManager::resolveHttpRedirection(dataAdapter));
 			addHeader(contentLength.append(Utils::getStringSizeStr(body.size())));
			break;
		case BAD_REQUEST:
			statusCode = "400";
			statusMsg = "BAD_REQUEST";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default400");
			FileManager::readFile(dataAdapter);
			break;
		case FORBIDDEN:
			statusCode = "403";
			statusMsg = "FORBIDDEN";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default403");
			FileManager::readFile(dataAdapter);
			break;
		case NOT_FOUND:
			statusCode = "404";
			statusMsg = "NOT_FOUND";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default404");
			FileManager::readFile(dataAdapter);
			break;
		case METHOD_NOT_ALLOWED:
			statusCode = "405";
			statusMsg = "METHOD_NOT_ALLOWED";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default405");
			FileManager::readFile(dataAdapter);
			break;
		case CONFLICT:
			statusCode = "409";
			statusMsg = "CONFLICT";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default409");
			FileManager::readFile(dataAdapter);
			break;
		case PAYLOAD_TOO_LARGE:
			statusCode = "413";
			statusMsg = "PAYLOAD_TOO_LARGE";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default413");
			FileManager::readFile(dataAdapter);
			break;
		case SERVER_ERROR:
			statusCode = "500";
			statusMsg = "SERVER_ERROR";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default500");
			FileManager::readFile(dataAdapter);
			break;
		case METHOD_NOT_IMPLEMENTED:
			statusCode = "501";
			statusMsg = "METHOD_NOT_IMPLEMENTED";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default501");
			FileManager::readFile(dataAdapter);
			break;
		case GATEWAY_TIMEOUT:
			statusCode = "504";
			statusMsg = "GATEWAY_TIMEOUT";
			dataAdapter.getRequest().url = PathManager::resolveErrorPage(dataAdapter, "default504");
			FileManager::readFile(dataAdapter);
			break;
		default:
			break;
	}
	std::cout << BLUE << "Info: " << statusCode << ", " << statusMsg << END << std::endl;
}