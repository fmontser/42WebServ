#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
#ifdef __APPLE__
	system("open new-years.gif");
#elif __linux__
	system("xdg-open 200w.gif");
	sleep(3);
	system("xdg-open new-years.gif");
#elif _WIN32
	system("start new-years.gif");
#endif
	std::cout << "Hello Wally & happy new Year" << std::endl;
	return 0;
}
