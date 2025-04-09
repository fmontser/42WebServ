#pragma once

#include <string>
#include "Connection.hpp"
#include "DataAdapter.hpp"


class PathManager
{
	static std::string _workDirectory;

	private:
		PathManager();
		PathManager(const PathManager& src);
		PathManager& operator=(const PathManager& src);
		~PathManager();
	public:

		static std::string	resolvePath(DataAdapter& dataAdapter, Connection* connection);
		static void			resolveHttpRedirection(DataAdapter& dataAdapter);
};