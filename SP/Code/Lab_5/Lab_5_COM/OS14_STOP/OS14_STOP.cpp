#include <iostream>
#include <Windows.h>


int main(int argc, char* argv[]) {
	if (argc != 1) {
		std::cerr << "Usage: ./OS14_STOP.exe " << std::endl;
		return EXIT_FAILURE;
	}

	const char* SuspendEventName = "Global\\Suspend_Event";
	HANDLE hSuspend = nullptr;

	while (!hSuspend) {
		hSuspend = OpenEventA(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, SuspendEventName);
		if (!hSuspend) {
			Sleep(100);
		}
	}

	if (!SetEvent(hSuspend)) {
		std::cerr << "SetEvent() for hSuspend failed. Error code: " << GetLastError() << std::endl;
		CloseHandle(hSuspend);
		return EXIT_FAILURE;
	}

	std::cout << "Suspend event signaled. All workers should stop and wait for Resume event" << std::endl;

	CloseHandle(hSuspend);
	return EXIT_SUCCESS;
}