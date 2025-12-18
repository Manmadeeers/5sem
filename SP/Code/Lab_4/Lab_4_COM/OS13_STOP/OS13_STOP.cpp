#include <iostream>
#include <Windows.h>

int main(int argc, char* argv[]) {
	if (argc != 1) {
		std::cerr << "Usage: ./OS13_STOP.exe" << std::endl;
		return 1;
	}
	HANDLE hStopEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		L"Stop"
	);

	SetEvent(hStopEvent);
}