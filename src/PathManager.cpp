#include "PathManager.hpp"
#include "Utils.hpp"
#include <unistd.h>
#include <limits.h>


	std::string PathManager::_workingDir;

	PathManager::PathManager() {}
	PathManager::PathManager(const PathManager& src) { (void)src; }
	PathManager::~PathManager() {}
	PathManager& PathManager::operator=(const PathManager& src) {
		(void)src;
		return *this;
	}

	void			PathManager::stackPath(std::string& path, std::string appendix) {

		if (!path.empty() && path.at(path.size() - 1) == '/')
			path.erase(path.size() - 1, 1);
		if (!appendix.empty() && appendix.at(0) == '/')
			appendix.erase(0,1);
		path.append("/");
		path.append(appendix);
	}

	void			PathManager::stackRelativePath(std::string& path, std::string appendix) {

		if (!path.empty() && path.at(path.size() - 1) == '/')
			path.erase(path.size() - 1, 1);
		if (!appendix.empty() && appendix.at(0) == '/')
			appendix.erase(0,1);
		path.append("/");
		if (path.at(0) == '/')
			path.erase(0,1);
		path.append(appendix);
	}

	std::string		PathManager::getWorkingDir() {
		char buffer[PATH_MAX] = {0};

		if(_workingDir.empty()) {
			getcwd(buffer, PATH_MAX);
			_workingDir = buffer;
		}
		return _workingDir;
	}

	void			PathManager::setWorkingDir(std::string dir) {
		char buffer[PATH_MAX] = {0};

		if (!dir.empty() && chdir(dir.c_str()) == 0) {
			getcwd(buffer, PATH_MAX);
			_workingDir = buffer;
		}
	}

	std::string		PathManager::resolveRoutePath(DataAdapter& dataAdapter) {
		Server			server = dataAdapter.getConnection()->getServer();
		HttpRequest		request = dataAdapter.getRequest();
		Route*			route = NULL; 
		std::string		path, url, _default;

		server.getRequestedRoute(&route, dataAdapter);
		if (route)
			_default = route->getDefault();

		url = request.url;
		if (request.isCgiRequest) {
			url = CgiAdapter::stripCgiParams(url);
			url = CgiAdapter::stripCgiPathInfo(url);
		}

		if (route) {
			if ((route->getRoot().empty() || route->getRoot().at(0) != '/')
					&& server.getRoot().at(0) != '/'
					&& route->getRoot() != "../")
				path.append(_workingDir);

			if (route->getRoot().empty())
				stackPath(path, server.getRoot());
			else
				stackPath(path, route->getRoot());

			stackPath(path, url);
			if (Utils::isDirectory(path) && !_default.empty() && request.method == "GET")
				stackPath(path, _default);
		} else
			stackPath(path, url);
		return (path);
	}


	std::string		PathManager::resolveServerPath(DataAdapter& dataAdapter) {
		Server			server = dataAdapter.getConnection()->getServer();
		HttpRequest		request = dataAdapter.getRequest();

		std::string		path, url;

		url = request.url;
		if (request.isCgiRequest) {
			url = CgiAdapter::stripCgiParams(url);
			url = CgiAdapter::stripCgiPathInfo(url);
		}

		path.append(_workingDir);
		stackPath(path, server.getRoot());

		stackPath(path, url);

		return (path);
	}

	std::string		PathManager::resolveUploadDir(DataAdapter& dataAdapter){
		Server			server = dataAdapter.getConnection()->getServer();
		Route*			route = NULL;
		std::string		upload, path;

		server.getRequestedRoute(&route, dataAdapter);
		if (route)
			upload = route->getUpload();

		if(upload.empty())
			return resolveRoutePath(dataAdapter);
		else if (upload.at(0) == '/')
			return upload;
		else {
			path = resolveServerPath(dataAdapter);
			stackPath(path, upload);
			return path;
		}

	}

	std::string		PathManager::resolveErrorPage(DataAdapter& dataAdapter, std::string defaultPage){
		std::string	path(_workingDir);

		stackPath(path, dataAdapter.getConnection()->getServer().getDefaults()[defaultPage]);
		return (path);
	}

	std::string		PathManager::resolveHttpRedirection(DataAdapter& dataAdapter){
		std::string	location = "Location: ";
		Route*		route = NULL;
		
		dataAdapter.getConnection()->getServer().getRequestedRoute(&route,dataAdapter);
		if (route)
			stackPath(location, route->getRedirect());
		
		return (location);
	}
