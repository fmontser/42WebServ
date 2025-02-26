#include "Utils.hpp"
#include <cstdlib>

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