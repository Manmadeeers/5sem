#include <iostream>
#include <Windows.h>


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: ./OS14_STOP.exe <FileName>" << std::endl;
		return EXIT_FAILURE;
	}

	const char* SuspendEventName = "Global\\Suspend_Event";
	HANDLE hSuspend = CreateEventA(
		nullptr,
		TRUE,
		FALSE,
		SuspendEventName
	);
	if (!hSuspend) {
		std::cerr << "CreateEventA() for hSuspend failed. Error code: " << GetLastError() << std::endl;
		return EXIT_FAILURE;
	}

	SetEvent(hSuspend);
	std::cout << "SuspendEvent set. Workers should stop and wait for storage to become available" << std::endl;
	return EXIT_SUCCESS;
}