#include <Windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
	constexpr int process_amount = 3;

	STARTUPINFOA process_startup_info[process_amount]{};
	PROCESS_INFORMATION process_info[process_amount]{};
	
	for (auto& s : process_startup_info) {
		s.cb = sizeof(STARTUPINFOA);
	}

	LPCSTR exe_file = "C:\\Users\\User-ee4aed31\\Desktop\\Lab3\\Lab-03x.exe";
	char cmd_line[256];
	sprintf_s(cmd_line, "Lab-03x.exe 10");

	BOOL created_process[process_amount]{ FALSE,FALSE,FALSE };

	created_process[0] = CreateProcessA(
		cmd_line,
		nullptr,
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&process_startup_info[0],
		&process_info[0]
	);

	created_process[1] = CreateProcessA(
		nullptr,
		cmd_line,
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&process_startup_info[1],
		&process_info[1]
	);

	created_process[2] = CreateProcessA(
		exe_file,
		(LPSTR)" 10",
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&process_startup_info[2],
		&process_info[2]
	);


	HANDLE process_handles[process_amount];
	int running = 0;

	for (int i = 0; i < process_amount; ++i) {
		if (created_process[i]) {
			process_handles[running++] = process_info[i].hProcess;
			std::cout << "Process number " << i + 1 << " created with PID: " << process_info[i].dwProcessId << std::endl;
		}
		else {
			std::cerr << "Error creating process number " << i + 1 << ". Error code: " << GetLastError() << std::endl;
		}
	}

	if (running > 0) {
		WaitForMultipleObjects(running, process_handles, TRUE, INFINITE);
	}
	for (int i = 0; i < process_amount; ++i) {
		if (created_process[i]) {
			CloseHandle(process_info[i].hThread);
			CloseHandle(process_info[i].hProcess);

		}
	}

	std::cout << "All processes finished" << std::endl;

	system("pause");
	return 0;

}