#pragma once
#include "HttpProcessor.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <sstream>

class DataAdapter;

class Index {
	private:
	//	std::string _path;

	public:
		Index();
		~Index();
		Index(const Index& src);
		Index& operator=(const Index& src);

		static std::vector<char> generateAutoindex(DataAdapter& adapter/* , Route *actualRoute */);
	};