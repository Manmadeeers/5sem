#include <iostream>
#include <Windows.h>


int main(int argc, char* argv[]) {
	constexpr int processAmount = 3;
	STARTUPINFOA processStartupInfo[processAmount]{};
	PROCESS_INFORMATION processInfo[processAmount]{};

	for (auto& s : processStartupInfo) {
		s.dwFillAttribute = sizeof(STARTUPINFO);
	}

	LPCSTR exeFile = "C:\\Users\\User-ee4aed31\\Desktop\\Lab3\\Lab-03x.exe";
	char cmdLine[256];
	sprintf_s(cmdLine, "Lab-03x.exe 10");

	BOOL createdProcess[processAmount]{ FALSE,FALSE,FALSE };

	createdProcess[0] = CreateProcessA(exeFile, nullptr,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&processStartupInfo[0], &processInfo[0]);

	createdProcess[1] = CreateProcessA(nullptr, cmdLine,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&processStartupInfo[1], &processInfo[1]);

	SetEnvironmentVariableA("ITER_NUM", "5");
	createdProcess[2] = CreateProcessA(exeFile, nullptr,
		nullptr, nullptr, FALSE, 0, nullptr, nullptr,
		&processStartupInfo[2], &processInfo[2]);

	HANDLE procHandles[processAmount];
	int running = 0;
	for (int i = 0; i < processAmount; ++i) {
		if (createdProcess[i]) {
			procHandles[running++] = processInfo[i].hProcess;
			std::cout << "Process num." << i + 1 << "created with PID: " << processInfo[i].dwProcessId << std::endl;
		}
		else {
			std::cerr << "Error creating a process num." << i + 1 << ", Error code: " << GetLastError() << std::endl;

		}
	}

	if (running > 0) {
		WaitForMultipleObjects(running, procHandles, TRUE, INFINITE);
	}

	for (int i = 0; i < processAmount; ++i) {
		if (createdProcess[i]) {
			CloseHandle(processInfo[i].hThread);
			CloseHandle(processInfo[i].hProcess);
		}
	}

	std::cout << "All processes finished" << std::endl;

	system("pause");
	return 0;
}