#include "Index.hpp"
#include "PathManager.hpp"
#include "Utils.hpp"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>

Index::Index() {}
Index::~Index() {}
Index::Index(const Index& src) {
	*this = src;
}
Index&	Index::operator=(const Index& src) {
	(void)src;
	return *this;
}
std::vector<char> Index::generateAutoindex(DataAdapter& dataAdapter) {
	std::string autoIndex;
	std::vector<char> body;
	
	std::string path = PathManager::resolveServerPath(dataAdapter);
	DIR *dir = opendir(path.c_str());
	struct dirent *entry;

	if (dir == NULL)
		return body;

	autoIndex += "<!DOCTYPE html>\n";
	autoIndex += "<html>\n<head>\n";
	autoIndex += "<title>Index of " + path + "</title>";
	autoIndex += "	<style>\n";
	autoIndex += "		body { font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }\n";
	autoIndex += "		.container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }\n";
	autoIndex += "		h1 { color: #333; margin-bottom: 20px; }\n";
	autoIndex += "		hr { border: 0; height: 1px; background: #ddd; margin: 20px 0; }\n";
	autoIndex += "		.file-list { width: 100%; border-collapse: collapse; }\n";
	autoIndex += "		.file-list th { text-align: left; padding: 10px; background: #f0f0f0; }\n";
	autoIndex += "		.file-list td { padding: 10px; border-bottom: 1px solid #eee; fileName}\n";
	autoIndex += "		.file-list tr:hover { background: #f9f9f9; }\n";
	autoIndex += "		a { color: #007BFF; text-decoration: none; }\n";
	autoIndex += "		a:hover { text-decoration: underline; }\n";
	autoIndex += "		.btn { padding: 5px 10px; border-radius: 4px; font-size: 12px; cursor: pointer; }\n";
	autoIndex += "		.download-btn { background: #28a745; color: white; border: none; }\n";
	autoIndex += "		.delete-btn { background: #dc3545; color: white; border: none; }\n";
	autoIndex += "		.home-link { display: inline-block; margin-bottom: 20px; }\n";
	autoIndex += "	</style>\n";

	autoIndex += "	<script>\n";
	autoIndex += "		function deleteFile(filename) {\n";
	autoIndex += "			if (confirm('Are you sure you want to delete ' + filename + '?')) {\n";
	autoIndex += "				fetch(filename, {\n";
	autoIndex += "					method: 'DELETE',\n";
	autoIndex += "					headers: { 'Content-Type': 'application/json' }\n";
	autoIndex += "				}).then(response => {\n";
	autoIndex += "					if (response.ok) {\n";
	autoIndex += "						window.location.reload();\n";
	autoIndex += "					} else {\n";
	autoIndex += "						alert('Failed to delete file');\n";
	autoIndex += "					}\n";
	autoIndex += "				});\n";
	autoIndex += "			}\n";
	autoIndex += "		}\n";
	autoIndex += "		function downloadFile(filename) {\n";
	autoIndex += "			window.location.href = filename + '?download=true';\n";
	autoIndex += "		}\n";
	autoIndex += "	</script>\n";



	autoIndex += "</head>\n<body>\n";
	autoIndex += "<div class=\"container\">\n";
	autoIndex += "	<h1>Index of " + path + "</h1>\n";
	autoIndex += "	<a href=\"http://localhost:8042\" class=\"home-link\">Back to parent directory</a>\n";
	autoIndex += "	<hr>\n";
	autoIndex += "	<table class=\"file-list\">\n";
	autoIndex += "	<thead><tr><th>Name</th><th>Actions</th></tr></thead>\n";
	autoIndex += "<tbody>\n";


	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] != '.') {
			std::string fileName = entry->d_name;

			//TODO arreglar raiz del/ == del (PATHMANAGER!!!!!)

			std::string relativePath;
			PathManager::stackRelativePath(relativePath, dataAdapter.getRequest().url);
			PathManager::stackRelativePath(relativePath, fileName);


			std::cout << "\t >>>" << relativePath << std::endl;


			autoIndex += "		<tr>\n";
			autoIndex += "			<td><a href=\"" + relativePath + "\" target=\"_blank\" >" + fileName + "</a></td>\n";
			autoIndex += "			<td>\n";
			autoIndex += "				<button class=\"btn download-btn\" onclick=\"downloadFile('" + relativePath + "')\">Download</button>\n";
			autoIndex += "				<button class=\"btn delete-btn\" onclick=\"deleteFile('" + relativePath + "')\">Delete</button>\n";
			autoIndex += "			</td>\n";
			autoIndex += "		</tr>\n";
		}
	}

	autoIndex += "		</tbody>\n";
	autoIndex += "	</table>\n";
	autoIndex += "	<hr>\n";
	autoIndex += "	<p style=\"text-align: right; color: #666;\">By courtesy of your friendly cpp 42WebServer</p>\n";
	autoIndex += "</div>\n";
	autoIndex += "</body>\n</html>\n";
	closedir(dir);
	return body = std::vector<char>(autoIndex.begin(), autoIndex.end());
}

bool	Index::isIndexRoute(DataAdapter& dataAdapter, Route *actualRoute) {
	std::string	path = PathManager::resolveServerPath(dataAdapter);
	HttpRequest	request = dataAdapter.getRequest();

	if (request.method == "GET"
		&& Utils::isDirectory(path)
		&& actualRoute->getAutoIndex() == "on"
		&& ((access(path.c_str(), R_OK) == 0))
		&& actualRoute->getDefault().empty())
			return true;
	return false;
}