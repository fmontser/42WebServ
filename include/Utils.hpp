#pragma once

#include <vector>
#include <string>

class Utils {
	public:
		static std::vector<std::string> splitString(const std::string& str, char delimiter);
		static void	trimString(std::string& str);
};