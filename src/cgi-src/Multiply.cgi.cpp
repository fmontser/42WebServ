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
	int									a, b, t, resultado;
	std::string							o;
	std::string							display_operator;

	a = atoi(params["a"].c_str());
	b = atoi(params["b"].c_str());
	t = atoi(params["t"].c_str());
	o = params["operator"];

	if (o == "*") {
		resultado = (a * b);
		display_operator = "*";
	}
	else if (o == "%2B") {
		resultado = (a + b);
		display_operator = "+";
	}
	else if (o == "-") {
		resultado = (a - b);
		display_operator = "-";
	}
	else if (o == "%2F") {
		if (b != 0) {
			resultado = (a / b);
			display_operator = "/";
		}
		else
			std::cerr << "<h1>Division by zero</h1>";
		
	}
	/* else
		resultado = 0; */
	sleep(t);
	

	std::cout << "<html><head>\n";
	std::cout << "<style>\n";
	std::cout << "  body { font-family: Arial, sans-serif; background-color: #f4f4f4; display: flex; align-items: center; justify-content: center; height: 100vh; margin: 0; }\n";
	std::cout << "  .container { background: white; padding: 20px 40px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }\n";
	std::cout << "  h1 { color: #2c3e50; }\n";
	std::cout << "  p { font-size: 18px; margin-bottom: 20px; }\n";
	std::cout << "  a { text-decoration: none; color: #3498db; font-weight: bold; }\n";
	std::cout << "</style>\n";
	std::cout << "<html><body>\n";
	std::cout << "<div class=\"container\">\n";
	std::cout << "<p>calculating " << a << " " << " " << display_operator << " "<< b << "</p>\n";
	std::cout << "<h1>Resultado: " << resultado << "</h1>\n";
	std::cout << "<a href=\"/\">Volver</a>" << std::endl;
	std::cout << "</div>\n";
	std::cout << "</body></html>\n";


	return (0);
}