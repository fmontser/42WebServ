#include "Route.hpp"

Route::Route() {}
Route::~Route() {}
Route::Route(const Route& src) {	
		this->_url = src._url;
		this->_methods = src._methods;
		this->_files = src._files;
}
Route& Route::operator=(const Route& src) {
	if (this != &src) {
		this->_url = src._url;
		this->_methods = src._methods;
		this->_files = src._files;
	}
	return *this;
}

std::string Route::getUrl() const {	return this->_url;}
std::map<std::string, std::string> Route::getMethods() const {	
	return this->_methods;
	}	
std::map<std::string, std::string> Route::getFiles() const {	
	return this->_files;
	}

void Route::setUrl(const std::string& url) {	this->_url = url;}
void Route::setMethods(std::map<std::string, std::string>& methods) {	
	this->_methods = methods;
	}
void Route::setFiles(std::map<std::string, std::string>& files) {	
	this->_files = files;
	}

