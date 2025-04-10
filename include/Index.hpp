#pragma once

#include "HttpProcessor.hpp"
#include <string>

class DataAdapter;

class Index {
	public:
		Index();
		~Index();
		Index(const Index& src);
		Index& operator=(const Index& src);

		static std::vector<char> generateAutoindex(DataAdapter& dataAdapter);
		static bool isIndexRoute(DataAdapter& adapter, Route *actualRoute);
	};