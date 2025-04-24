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
	std::cout << "<html><head>\n";
	std::cout << "<style>\n";
	std::cout << "  body { font-family: Arial, sans-serif; background-color: #f4f4f4; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; }\n";
	std::cout << "  .container { background: white; padding: 30px 40px; border-radius: 12px; box-shadow: 0 0 15px rgba(0,0,0,0.1); text-align: center; max-width: 800px; }\n";
	std::cout << "  pre { text-align: left; background-color: #f0f0f0; padding: 15px; border-radius: 8px; overflow: auto; }\n";
	std::cout << "  a { display: inline-block; margin-top: 20px; text-decoration: none; color: #3498db; font-weight: bold; }\n";
	std::cout << "</style>\n";
	std::cout << "</head><body>\n";

	std::cout << "<div class=\"container\">\n";
	std::cout << "<h2>Contenido procesado</h2>\n";
	std::cout << "<pre>" << body << "</pre>\n";
	std::cout << "<a href=\"/\">Volver</a>\n";
	std::cout << "</div>\n";

	std::cout << "</body></html>\n";

	return (EXIT_SUCCESS);
}


