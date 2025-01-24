#include "Route.hpp"

Route::Route() {}
Route::~Route() {}
Route::Route(const Route& src) {	
	_url = src._url;
	_methods = src._methods;
	_files = src._files;
}
Route& Route::operator=(const Route& src) {
	if (this != &src) {
		_url = src._url;
		_methods = src._methods;
		_files = src._files;
	}
	return *this;
}

std::string Route::getUrl() const {	return this->_url;}
std::map<std::string, std::string> Route::getMethods() const { return this->_methods; }
std::map<std::string, std::string> Route::getFiles() const { return this->_files; }

void Route::setUrl(const std::string& url) { this->_url = url;}
void Route::addMethod(const std::pair<std::string, std::string>& method) { _methods.insert(method); }
void Route::addFile(const std::pair<std::string, std::string>& file) { _files.insert(file); }