#include <iostream>
#include <string>
#include <Windows.h>
#define PIPE_NAME "\\\\.\\pipe\\Tube"
#define NETWORK_PIPE_NAME "\\\\DESKTOP-I\\pipe\\Tube"

std::string SetPipeError(std::string message, int code) {
	return message + ".Error: " + std::to_string(code);
}

int main(int argc, char* argv[]) {

	HANDLE hPipe;
	try {
		std::cout << "--Waiting for named pipe to be available" << std::endl;

		while (true) {
			if (WaitNamedPipeA(PIPE_NAME, INFINITE)) {
				std::cout << "--Named pipe available. Client may proceed" << std::endl;
				break;
			}

		}

		char call_message[128] = "Hello from CallNamedPipe()";
		char read_call[128];

		DWORD read_bytes = 0;
		
		DWORD call_result = CallNamedPipeA(
			PIPE_NAME,
			call_message,
			(DWORD)sizeof(call_message),
			read_call,
			(DWORD)sizeof(read_call),
			&read_bytes,
			NMPWAIT_WAIT_FOREVER
		);
		if (!call_result) {
			throw SetPipeError("CallNamedPipe() function failed", GetLastError());
		}
		if ((int)read_bytes < (int)sizeof(read_call)) {
			read_call[(int)read_bytes] = '\0';
		}

		std::cout << "--Written via CallNamedPipe: " << call_message << std::endl;
		std::cout << "--Read via CallNamedPipe: " << read_call << std::endl;

	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}
	system("pause");
	return 0;
}