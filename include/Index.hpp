#pragma once

#include <string>
#include "HttpProcessor.hpp"

class DataAdapter;

class Index {
	public:
		Index();
		~Index();
		Index(const Index& src);
		Index& operator=(const Index& src);

		static std::vector<char>	generateAutoindex(DataAdapter& dataAdapter);
		static bool					isIndexRoute(DataAdapter& dataAdapter, Route *actualRoute);
	};