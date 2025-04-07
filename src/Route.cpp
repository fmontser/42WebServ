#include "Route.hpp"

Route::Route() {}
Route::~Route() {}
Route::Route(const Route& src) {	
	_url = src._url;
	_methods = src._methods;
	_default = src._default;
	_root = src._root;
	_redirect = src._redirect;
	_autoIndex = src._autoIndex;

}
Route& Route::operator=(const Route& src) {
	if (this != &src) {
		_url = src._url;
		_methods = src._methods;
		_default = src._default;
		_root = src._root;
		_redirect = src._redirect;
		_autoIndex = src._autoIndex;
	}
	return *this;
}

std::string							Route::getUrl() const {	return this->_url;}
std::string							Route::getAutoIndex() const { return _autoIndex; }
std::string							Route::getRoot() const { return _root; }
std::string							Route::getRedirect() const { return _redirect; }
std::string							Route::getDefault() const { return _default; }
std::multimap<std::string, std::string>	Route::getMethods() const { return this->_methods; }

void	Route::setUrl(const std::string& url) { this->_url = url;}
void	Route::addMethod(const std::pair<std::string, std::string> method) { _methods.insert(method); }
void	Route::setAutoIndex(const std::string autoIndex) { _autoIndex = autoIndex; }
void	Route::setRoot(const std::string root) { _root = root; }
void	Route::setRedirect(const std::string redirect) { _redirect = redirect; }
void	Route::setDefault(const std::string default_) { _default = default_; }

//TODO check!
bool	Route::isMethodAllowed(std::string method) {
	for (std::multimap<std::string, std::string>::iterator it = _methods.begin(); it != _methods.end(); ++it) {
		if (it->second == method)
			return true;
	}
	return false;
}