#include "Index.hpp"

Index::Index() {}
Index::~Index() {}
Index::Index(const Index& src) {
	*this = src;
}
Index& Index::operator=(const Index& src) {
	(void)src;
	/* if (this != &src)
		_path = src._path; */
	return *this;
}
std::vector<char> Index::generateAutoindex(DataAdapter& adapter) {
	std::string autoIndex;
	std::vector<char> body;
	
	
	std::string path = std::string("..").append(adapter.getConnection()->getServer().getRoot() + adapter.getRequest().url);
	DIR *dir = opendir(path.c_str());
	struct dirent *entry;

	if (dir == NULL)
		return body;

	autoIndex += "<html><head><title>Index of " + path + "</title></head><body>";
	autoIndex += "<style>body { font-family: Arial, sans-serif; }</style>";
	autoIndex += "<style>pre { font-family: monospace; }</style>";
	autoIndex += "<style>h1 { color: #333; }</style>";
	autoIndex += "<style>hr { border: 1px solid #ccc; }</style>";
	autoIndex += "<style>a { text-decoration: none; color: #007BFF; }</style>";
	autoIndex += "<style>a:hover { text-decoration: underline; }</style>";
	autoIndex += "<h1>Index of " + path + "</h1><hr><pre>";
	autoIndex += "<a href=\"..\">Home</a>\n";
	autoIndex += "<hr> <br>";

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] != '.') {
			autoIndex += "<a href=\"" + std::string(entry->d_name) + "\">" + std::string(entry->d_name) + "</a>\n";
		}
	}

	autoIndex += "</pre><hr></body></html>";
	closedir(dir);
	return body = std::vector<char>(autoIndex.begin(), autoIndex.end());
}