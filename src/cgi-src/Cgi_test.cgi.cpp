#include <iostream>
#include <string>
#include <cstdlib>

int main(void)
{
	std::cout << "Content-Type: text/html\r\n\r\n";

	const char* contentLengthStr = std::getenv("CONTENT_LENGTH");
	if (!contentLengthStr)
	{
		std::cout << "<html><body><h1>No POST data received</h1></body></html>";
		return 0;
	}

	int contentLength = std::atoi(contentLengthStr);
	if (contentLength <= 0)
	{
		std::cout << "<html><body><h1>Invalid Content-Length</h1></body></html>";
		return 0;
	}

	std::string postData;
	postData.resize(contentLength);

	std::cin.read(&postData[0], contentLength);

	std::cout << "<html><body>";
	std::cout << "<h1>POST Data Received:</h1>";
	std::cout << "<pre>" << postData << "</pre>";
	std::cout << "</body></html>";

	return 0;
}
