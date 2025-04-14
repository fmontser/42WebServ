#pragma once

#include <string>
#include "Connection.hpp"
#include "DataAdapter.hpp"

class PathManager
{
	static std::string _workingDir;

	private:
		PathManager();
		PathManager(const PathManager& src);
		PathManager& operator=(const PathManager& src);
		~PathManager();
	public:

		static std::string	getWorkingDir();
		static void			setWorkingDir(std::string dir);

		static void			stackPath(std::string& path, std::string appendix);
		static void			stackRelativePath(std::string& path, std::string appendix);
		static std::string	resolveRoutePath(DataAdapter& dataAdapter);
		static std::string	resolveServerPath(DataAdapter& dataAdapter);
		static std::string	resolveErrorPage(DataAdapter& dataAdapter, std::string defaultPage);
		static std::string	resolveHttpRedirection(DataAdapter& dataAdapter);
};