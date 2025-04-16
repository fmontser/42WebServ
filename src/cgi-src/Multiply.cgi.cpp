#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h>

// Función para parsear la cadena de consulta
std::map<std::string, std::string> parseParameters(const std::string& str) {
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
	// Obtener la cadena de consulta
	const char* queryString = std::getenv("QUERY_STRING");
	std::string query = (queryString != NULL) ? queryString : "";

	// Parsear los parámetros
	std::map<std::string, std::string> params = parseParameters(query);

	// Obtener los valores de a y b
	int a = 0;
	int b = 0;
	int t = 0;
	try {
		a = std::atoi(params["a"].c_str());
		b = std::atoi(params["b"].c_str());
		t = std::atoi(params["t"].c_str());
	} catch (const std::exception& e) {
		std::cerr << "Error: Invalid input" << std::endl;
		return 1;
	}

	// Calcular el resultado
	int resultado = a * b;

	//TEST procesamiento largo
	sleep(t);

	// Imprimir el resultado en stdout
	std::cout << "<html><body>\n";
	std::cout << "<h1>Resultado: " << resultado << "</h1>\n";
	std::cout << "</body></html>\n";

	return 0;
}