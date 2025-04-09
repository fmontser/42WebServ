#include "PathManager.hpp"
#include "Utils.hpp"

	std::string PathManager::_workDirectory = "../";

	PathManager::PathManager() {}
	PathManager::PathManager(const PathManager& src) { (void)src; }
	PathManager::~PathManager() {}
	PathManager& PathManager::operator=(const PathManager& src) {
		(void)src;
		return *this;
	}

	static void appendPath(std::string& path, std::string appendix) {
		if (path.empty() || appendix.empty())
			return ;

		if (path.at(path.size() - 1) == '/') {
			if (appendix.at(0) == '/')
				appendix.erase(appendix.begin());
		} else if (appendix.at(0) != '/') {
			appendix = std::string("/").append(appendix);
		}
		path.append(appendix);
	}

	//TODO probar todas las conbianciones de / antes y despues de url, directorios...
	std::string	PathManager::resolvePath(DataAdapter& dataAdapter) {
		Server&			server = dataAdapter.getConnection()->getServer();
		HttpRequest&	request = dataAdapter.getRequest();
		Route*			route = server.getRequestedRoute(Utils::getUrlPath(request.url));
		std::string		_default(route->getDefault());
		std::string		path;

		if (route) {
			if ((route->getRoot().empty() || route->getRoot().at(0) != '/')
					&& server.getRoot().at(0) != '/') //TODO check!
				path.append(_workDirectory);

			if (route->getRoot().empty())
				appendPath(path, server.getRoot());
			else
				appendPath(path, route->getRoot());

			appendPath(path, request.url);
			if (Utils::isDirectory(path) && !_default.empty())
				appendPath(path, _default);
		} else
			appendPath(path, request.url);
		return (path);
	}


	std::string	PathManager::resolveErrorPage(DataAdapter& dataAdapter, std::string defaultPage){
		std::string	path(_workDirectory);

		path.append(dataAdapter.getConnection()->getServer().getDefaults()[defaultPage]);
		return (path);
	}

	void		PathManager::resolveHttpRedirection(DataAdapter& dataAdapter){
		(void)dataAdapter;

		//TODO implementar
	}
