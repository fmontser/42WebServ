#include "HttpResponse.hpp"
#include "FileManager.hpp"
#include "ServerConstants.hpp"
#include "Utils.hpp"
#include "DataAdapter.hpp"
#include <iostream>
#include <sstream>
#include "Index.hpp"

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

bool	HttpResponse::isChunked() {
	for (std::vector<HttpHeader>::iterator it = headers.begin(); it != headers.end(); ++it) {
		HttpHeader header = *it;
		HeaderValue value;

		if (header.getValue("Transfer-Encoding", &value) && value.name == "chunked")
			return true;
	}
	return false;
}



void	HttpResponse::setupResponse(enum responseType responseType, DataAdapter& dataAdapter) {
	std::string contentLength ("Content-Length: ");

	version = HTTP_VERSION;
	switch (responseType) {
		case DIR_LIST:
			statusCode = "200";
			statusMsg = "OK";

			body = Index::generateAutoindex(dataAdapter);
			//@@@@@@@@ listing! body = CLASE listing (USAR DATADAPTER COMO PARAMETRO)

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
			break;
		case CREATED:
			statusCode = "201";
			statusMsg = "CREATED";
			dataAdapter.getRequest().url = "/defaults/201.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case NO_CONTENT:
			statusCode = "204";
			statusMsg = "NO_CONTENT";
			break;
		case SEE_OTHER:
 			statusCode = "303";
			statusMsg = "SEE_OTHER";
			addHeader("Location: /");
 			addHeader(contentLength.append(Utils::getStringSizeStr(body.size())));
			break;
		case BAD_REQUEST:
			statusCode = "400";
			statusMsg = "BAD_REQUEST";
			dataAdapter.getRequest().url = "/defaults/400.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case FORBIDDEN:
			statusCode = "403";
			statusMsg = "FORBIDDEN";
			dataAdapter.getRequest().url = "/defaults/403.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case NOT_FOUND:
			statusCode = "404";
			statusMsg = "NOT_FOUND";
			dataAdapter.getRequest().url = "/defaults/404.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case CONFLICT:
			statusCode = "409";
			statusMsg = "CONFLICT";
			dataAdapter.getRequest().url = "/defaults/409.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case METHOD_NOT_ALLOWED:
			statusCode = "405";
			statusMsg = "METHOD_NOT_ALLOWED";
			dataAdapter.getRequest().url = "/defaults/405.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case PAYLOAD_TOO_LARGE:	//TODO TEST
			statusCode = "413";
			statusMsg = "PAYLOAD_TOO_LARGE";
			dataAdapter.getRequest().url = "/defaults/413.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case SERVER_ERROR:
			statusCode = "500";
			statusMsg = "SERVER_ERROR";
			dataAdapter.getRequest().url = "/defaults/500.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		case METHOD_NOT_IMPLEMENTED:
			statusCode = "501";
			statusMsg = "METHOD_NOT_IMPLEMENTED";
			dataAdapter.getRequest().url = "/defaults/501.html";
			FileManager::readFile(dataAdapter,NULL);
			break;
		default:
			break;
	}
	std::cout << BLUE << "Info: " << statusCode << ", " << statusMsg << END << std::endl;
}