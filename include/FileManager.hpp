#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class DataAdapter;

class FileManager {
	private:

		FileManager();
		~FileManager();
		FileManager(const FileManager& src);
		FileManager& operator=(const FileManager& src);

	public:

		static void readFile(DataAdapter& dataAdapter);
		static void writeFile(DataAdapter& dataAdapter);
};