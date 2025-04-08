#pragma once

#include <vector>
#include <string>

class Utils {
	public:
		static std::vector<std::string>	splitString(const std::string& str, char delimiter);
		static void						trimString(std::string& str);
		static size_t					strToUint(std::string str);
		static size_t					strHexToUint(std::string  str);
		static void						nestedQuoteExtract(char delimiter, std::string& str);
		static size_t					getStringStreamLength(std::stringstream& ss);
		static std::string				getStringSizeStr(size_t size);
		static std::string				getUrlPath(std::string url);
		static bool						isDirectory(std::string path);
};