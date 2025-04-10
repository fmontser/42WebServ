#pragma once
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Route.hpp"

class DataAdapter;

class FileManager {
	private:

		FileManager();
		~FileManager();
		FileManager(const FileManager& src);
		FileManager& operator=(const FileManager& src);

	public:

		static HttpResponse::responseType	readFile(DataAdapter& dataAdapter, Route* actualRoute);
		static HttpResponse::responseType	writeFile(DataAdapter& dataAdapter, Route* actualRoute);
		static HttpResponse::responseType	deleteFile(DataAdapter& dataAdapter, Route* actualRoute);
		static HttpResponse::responseType downloadFile(DataAdapter& adapter, Route* route);//new
		
};