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
	int									a, b, t;

	a = atoi(params["a"].c_str());
	b = atoi(params["b"].c_str());
	t = atoi(params["t"].c_str());

	int resultado = (a * b);
	sleep(t);

	std::cout << "<html><body>\n";
	std::cout << "<h1>Resultado: " << resultado << "</h1>\n";
	std::cout << "</body></html>\n";

	return (0);
}