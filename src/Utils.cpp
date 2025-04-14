#include "Utils.hpp"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <algorithm>


std::vector<std::string> Utils::splitString(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		token = str.substr(start, end - start);
		tokens.push_back(token);
		start = end + 1;
		end = str.find(delimiter, start);
	}
	token = str.substr(start);
	tokens.push_back(token);
	return tokens;
}

void	Utils::trimString(std::string& str) {
	size_t start = 0;
	size_t end = str.length() - 1;

	while (start <= end && std::isspace(str[start])) {
		start++;
	}
	while (end >= start && (std::isspace(str[end]) || str[end] == '\r')) {
		end--;
	}
	str = str.substr(start, end - start + 1);
}

size_t	Utils::strToUint(std::string str) {
	return std::strtol(str.c_str(), NULL, 10);
}

size_t	Utils::strHexToUint(std::string  str) {
	return std::strtol(str.c_str(), NULL, 16);
}

void	Utils::nestedQuoteExtract(char delimiter, std::string& str) {
	size_t start, end;

	while (str.find(delimiter,0) != str.npos) {
		start = str.find(delimiter,0);
		end = str.find_last_of(delimiter, str.size());
		str = str.substr(start + 1, end - 1);
	}
}


size_t	Utils::getStringStreamLength(std::stringstream& ss) {
	std::streampos pos = ss.tellg();
	ss.seekg(0, std::ios::end);
	std::streampos length = ss.tellg();
	ss.seekg(pos);
	return length;
}

std::string Utils::getStringSizeStr(size_t size) {
	std::stringstream ss;
	
	ss << size;
	return ss.str();
}

std::string	Utils::getUrlPath(std::string url) {
	std::string path;
	path = url.substr(0, url.find_last_of('/', url.size()));
	if (path.empty())
		path.append("/");
	return path;
}

bool	Utils::isDirectory(std::string path) {
	struct stat file_info;

	if (stat(path.c_str(), &file_info) == 0) {
		if (S_ISDIR(file_info.st_mode))
			return true;
	}
	return false;
}
std::string Utils::getFileType(std::string path) {
	size_t pos = path.find_last_of('.');
	if (pos != std::string::npos) {
		std::string ext = path.substr(pos + 1);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		return ext;
	}
	return "";
}

std::string Utils::toString(size_t value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}