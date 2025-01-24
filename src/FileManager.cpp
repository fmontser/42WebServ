#include "FileManager.hpp"

HttpRequest		FileManager::_request;
HttpResponse	FileManager::_response;

FileManager::FileManager() {
	//TODO
}

FileManager::~FileManager() {
	//TODO
}

FileManager::FileManager(const FileManager& src) {
	//TODO
	(void)src;
}

FileManager& FileManager::operator=(const FileManager& src) {
	//TODO
	(void)src;
	return *this;
}

void	FileManager::processHttpRequest() {
	//TODO
}

bool	FileManager::searchFile() {
	//TODO
	return false;
}

bool	FileManager::createFile() {
	//TODO
	return false;
}

bool	FileManager::deleteFile() {
	//TODO
	return false;
}

bool	FileManager::controlAccess() {
	//TODO
	return false;
}

void	FileManager::recieveHttpRequest(HttpRequest& request) {
	//TODO
	(void)request;
}

void	FileManager::recieveHttpResponse(HttpResponse& response) {
	//TODO
	(void)response;
}

