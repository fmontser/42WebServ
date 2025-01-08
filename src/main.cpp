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
//	system("open new-years.gif");
	system("osascript -e 'tell application \"Preview\" to open POSIX file \"new-years.gif\"' -e 'set the bounds of window 1 to {100, 100, 500, 500}'");
	system("osascript -e 'tell application \"Preview\" to open POSIX file \"200w.gif\"' -e 'set the bounds of window 1 to {600, 100, 1000, 500}'");
#elif __linux__
	system("xdg-open 201w.gif &");
	sleep(3);
	system("xdg-open 200w.gif &");
	sleep(3);
//	system("xdotool search --sync --name 200w.gif windowmove 100 100");
	system("xdg-open new-years.gif &");
//	system("xdotool search --sync --name new-years.gif windowmove 600 100");
#elif _WIN32
	system("start new-years.gif");
#endif
	std::cout << "Hello Wally & happy new Year" << std::endl;
	incrementCounter();
	incrementCounter();
	incrementCounter();
	return 0;
}
