#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <map>
#include <unistd.h> // Para sleep
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Función para parsear la cadena de consulta (QUERY_STRING)
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

bool isStdinRedirected() {
	struct stat st;
	if (fstat(STDIN_FILENO, &st) == 0) {
		return S_ISFIFO(st.st_mode); // Check if stdin is a pipe
	}
	return false; // Assume not redirected if fstat fails
}

int main() {
	// Obtener la cadena de consulta (QUERY_STRING) del entorno.
	const char* queryString = std::getenv("QUERY_STRING");
	std::string query = (queryString != NULL) ? queryString : "";
	std::string body;

	// Check if stdin is redirected (i.e., data is being piped in)
	if (isStdinRedirected()) {
		// Leer desde stdin (el pipe)
		std::string line;
		while (std::getline(std::cin, line)) {
			body += line + "\n";  // Añade cada línea al body, incluyendo el newline.
		}
	} else {
		// Si no hay stdin, dejamos el body vacío.  Podrías poner un mensaje por defecto.
		std::cerr << "No data received from stdin (pipe)." << std::endl;
		//body = "No data provided via stdin."; // Opcional: Mensaje por defecto.
	}


	// Parsear los parámetros de la QUERY_STRING
	std::map<std::string, std::string> params = parseParameters(query);

	// Obtener los valores de a, b y t desde la QUERY_STRING.  Manejo de errores mejorado.
	int a = 0;
	int b = 0;
	int t = 0;
	std::stringstream ss_a(params["a"]);
	std::stringstream ss_b(params["b"]);
	std::stringstream ss_t(params["t"]);

	if (!(ss_a >> a)) {
		std::cerr << "Error: Invalid input for 'a'" << std::endl;
		return 1;
	}

	if (!(ss_b >> b)) {
		std::cerr << "Error: Invalid input for 'b'" << std::endl;
		return 1;
	}
	if (!(ss_t >> t)) {
		std::cerr << "Error: Invalid input for 't'" << std::endl;
		return 1;
	}


	// Calcular el resultado
	int resultado = (a * b) + body.size();

	//Simular un procesamiento largo
	sleep(t);

	// Imprimir el resultado en stdout
	std::cout << "Content-Type: text/html\r\n\r\n"; //Encabezado HTTP necesario para CGI
	std::cout << "<html><body>\n";
	std::cout << "<h1>Resultado: " << resultado << "</h1>\n";
	std::cout << "</body></html>\n";

	return 0;
}