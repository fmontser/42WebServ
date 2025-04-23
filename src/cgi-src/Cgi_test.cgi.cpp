#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>

std::map<std::string, std::string> parseParameters(std::string& str) {
	std::map<std::string, std::string> parameters;
	std::stringstream ss(str);
	std::string pair;

	while (std::getline(ss, pair, '&')) {
		size_t pos = pair.find('=');
		if (pos != std::string::npos) {
			std::string name = pair.substr(0, pos);
			std::string value = pair.substr(pos + 1);
			parameters[name] = value;
		}
	}
	return parameters;
}

int main() {
	std::string body;
	const char* requestMethod = std::getenv("REQUEST_METHOD");
	const char* queryString = std::getenv("QUERY_STRING");
	const char* contentLengthStr = std::getenv("CONTENT_LENGTH");

	std::string query = (queryString != NULL) ? queryString : "";
	std::map<std::string, std::string> params = parseParameters(query);

	int contentLength = (contentLengthStr != NULL) ? std::atoi(contentLengthStr) : 0;

	if (requestMethod && std::string(requestMethod) == "POST" && contentLength > 0) {
		body.resize(contentLength);
		std::cin.read(&body[0], contentLength);
		std::map<std::string, std::string> bodyParams = parseParameters(body);

		std::cout << "Content-Type: text/html\r\n\r\n";
		std::cout << "<html><body>";
		std::cout << "<p><a href=\"/\">Volver</a></p>";
		std::cout << "<h3>POST Body (modified '0' → '2'):</h3>";

		// Modify and print body
		for (std::string::iterator it = body.begin(); it != body.end(); ++it) {
			if (*it == '0')
				*it = '2';
		}
		std::cout << "<pre>" << body << "</pre>";

		std::cout << "<h3>Parsed POST Parameters:</h3><ul>";
		for (std::map<std::string, std::string>::iterator it = bodyParams.begin(); it != bodyParams.end(); ++it) {
			std::cout << "<li>" << it->first << " = " << it->second << "</li>";
		}
		std::cout << "</ul>";
		std::cout << "</body></html>";
	}
	else {
		std::cout << "Content-Type: text/html\r\n\r\n";
		std::cout << "<html><body>";
		std::cout << "<p><a href=\"/\">Volver</a></p>";
		std::cout << "<h3>GET Parameters:</h3><ul>";
		for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); ++it) {
			std::cout << "<li>" << it->first << " = " << it->second << "</li>";
		}
		std::cout << "</ul>";
		std::cout << "</body></html>";
	}

	return EXIT_SUCCESS;
}
