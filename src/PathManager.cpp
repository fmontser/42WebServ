#include "PathManager.hpp"
#include "Utils.hpp"

	static std::string _workDirectory = "../";

	PathManager::PathManager() {}
	PathManager::PathManager(const PathManager& src) { (void)src; }
	PathManager::~PathManager() {}
	PathManager& PathManager::operator=(const PathManager& src) {
		(void)src;
		return *this;
	}

	std::string PathManager::resolvePath(DataAdapter& dataAdapter, Connection* connection) {
		Server		server = connection->getServer();
		HttpRequest request = dataAdapter.getRequest();
		Route		route = *(server.getRequestedRoute(Utils::getUrlPath(request.url)));
		std::string path;

		if (!(route.getRoot()[0] == '/' || server.getRoot()[0] == '/')) //TODO check!
			path.append(_workDirectory);

		if (route.getRoot().empty())
			path.append(server.getRoot());
		else
			path.append(route.getRoot());

		path.append(request.url);
	}

	void			PathManager::resolveHttpRedirection(DataAdapter& dataAdapter){
		(void)dataAdapter;
		//TODO implementar
	}
