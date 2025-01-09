#include <iostream>
#include <cstdlib>
#include <unistd.h>

namespace {
    int counter = 0; // This variable is private to this file

    void incrementCounter() {
        ++counter;
        std::cout << "Counter: " << counter << std::endl;
    }
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
#ifdef __APPLE__
	system("open new-years.gif");
	sleep(3);
	system("open 201w.gif");
	sleep(3);
	system("open 200w.gif");
#elif __linux__
	system("xdg-open 201w.gif &");
	sleep(3);
	system("xdg-open 200w.gif &");
	sleep(3);
	system("xdg-open new-years.gif &");
#elif _WIN32
	system("start new-years.gif");
#endif
	std::cout << "Hello Wally & happy new Year" << std::endl;
	incrementCounter();
	incrementCounter();
	incrementCounter();
	return 0;
}
