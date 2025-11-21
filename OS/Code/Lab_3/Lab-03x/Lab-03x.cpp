#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <string>
#include <cstdlib>
int main(int argc, char* argv[]) {
	int iterationsCount = -1;
	if (argc == 2) {
		try {
			iterationsCount = std::atoi(argv[1]);
		}
		catch (...) {
			std::cerr << "Error: Invalid arguments format" << std::endl;
			ExitProcess(2);
		}
	}
	else {
		const char* env = std::getenv("ITER_NUM");
		if (env != nullptr) {
			try {
				iterationsCount = std::atoi(env);
			}
			catch (...) {
				std::cerr << "Error: ITER_NUM env invalid format" << std::endl;
				ExitProcess(3);
			}
		}
	}
	if (iterationsCount == -1) {
		std::cerr << "Error: Iterations count not set" << std::endl;
		ExitProcess(1);
	}
	std::cout << "Iterations count: " << iterationsCount << std::endl;
	DWORD pid = GetCurrentProcessId();
	for (int i = 0; i < iterationsCount; i++) {
		std::cout << "PID: " << pid << " Iteration " << i + 1 << "/" << iterationsCount << std::endl;
		Sleep(500);
	}

	system("pause");
	return 0;
}