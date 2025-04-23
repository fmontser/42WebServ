#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>


std::map<std::string, std::string> parseParameters(std::string& str) {
	std::map<std::string, std::string>	parameters;
	std::stringstream					ss(str);
	std::string							pair;

	while (std::getline(ss, pair, '&')) {
		size_t pos = pair.find('=');
		if (pos != std::string::npos) {
			std::string name = pair.substr(0, pos);
			std::string value = pair.substr(pos + 1);
			parameters[name] = value;
		}
	}
	return (parameters);
}


int main() {
	const char*							queryString = std::getenv("QUERY_STRING");
/* 	const char*							contentLengthStr = std::getenv("CONTENT_LENGTH");
	int									contentLength = (contentLengthStr != NULL) ? atoi(contentLengthStr) : 0; */
	std::string							query = (queryString != NULL) ? queryString : "";
	std::map<std::string, std::string>	params = parseParameters(query);
	std::string							body;
	char								readBuffer[4096] = {0};
	ssize_t								bytesRead;



	while ((bytesRead = read(STDIN_FILENO, readBuffer, sizeof(readBuffer))) > 0) {
		body.append(readBuffer, bytesRead);
	}

	for (std::string::iterator it = body.begin(); it != body.end(); ++it) {
		if (*it == '0')
			*it = '2';
	}
	std::cout << "<a href=\"/\">Volver</a>" << std::endl;
	std::cout << std::endl;
	std::cout << body;
	return (EXIT_SUCCESS);
}


