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

		hPipe = CreateFileA(
			PIPE_NAME,//pipe symbolic name
			GENERIC_READ | GENERIC_WRITE,//file access mode
			FILE_SHARE_READ | FILE_SHARE_WRITE,//shared use 
			NULL,//security attributes
			OPEN_EXISTING,//open mode
			NULL,//flags and attributes
			NULL//additional attributes
		);
		if (hPipe == INVALID_HANDLE_VALUE) {
			throw SetPipeError("Failed to open existing named pipe handle", GetLastError());
		}
		std::cout << "--Named pipe handle opened" << std::endl;

		DWORD mode = PIPE_READMODE_MESSAGE;
		if (!SetNamedPipeHandleState(
			hPipe,//named pipe handle
			&mode,//mode that's being set	
			NULL,//max collection count
			NULL//data collection timeout
		)) {
			throw SetPipeError("SetNamedPipeHandleState() function failed", GetLastError());
		}
		std::cout << "Named pipe handle set to PIPE_READMODE_MESSAGE mode" << std::endl;

		char transact_message[128] = "Message sent via TransactNamedPipe()";
		char read_transact[128];
		DWORD read_bytes = 0;

		BOOL transact_result = TransactNamedPipe(
			hPipe,//named pipe handle
			transact_message,//write buffer
			(DWORD)sizeof(transact_message),//size of write buffer
			read_transact,//read buffer
			(DWORD)sizeof(read_transact),//size of read buffer
			&read_bytes,//amount of actually read bytes
			NULL//for synchronous/asynchronous operations
		);

		if (!transact_result) {
			throw SetPipeError("Failed to commit a transaction", GetLastError());
		}

		if ((int)read_bytes < (int)sizeof(read_transact)) {
			read_transact[(int)read_bytes] = '\0';
		}

		std::cout << "--Written via transaction: " << transact_message << std::endl;
		std::cout << "--Read via transaction: " << read_transact << std::endl;


		if (CloseHandle(hPipe) == 0) {
			throw SetPipeError("Failed to close named pipe handle", GetLastError());
		}
		std::cout << "--Named pipe handle closed" << std::endl;
	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}

	system("pause");
	return 0;
}