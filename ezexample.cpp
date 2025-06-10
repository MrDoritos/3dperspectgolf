//Ez example
//g++ ezexample.cpp console.windows.cpp -o ezexample.exe
#include "console.h"

int main() {
	for (int x = 0; x < console::getConsoleWidth(); x++) {
		for (int y = 0; y < console::getConsoleHeight(); y++) {
			console::write(x,y,"0123456789"[(x % 10) + y % 10]);
		}
	}
	console::sleep(-1);
}