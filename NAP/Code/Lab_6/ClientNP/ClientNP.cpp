#include <iostream>
#include <string>
#include <Windows.h>
#define PIPE_NAME "\\\\.\\pipe\\Tube"
#define NETWORK_PIPE_NAME "\\DESKTOP-I\\pipe\\Tube"

std::string SetPipeError(std::string message, int code) {
	return message + ".Error: " + std::to_string(code);
}

const std::string MESSAGE = "Hello from client";

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

		char write_buffer[128];
		strcpy_s(write_buffer, sizeof(write_buffer), MESSAGE.c_str());
		DWORD bytes_to_write = (DWORD)sizeof(write_buffer);
		DWORD written_bytes = 0;

		BOOL writeFile_result = WriteFile(
			hPipe,//pipe handle
			write_buffer,//out buffer pointer
			bytes_to_write,//bytes to read
			&written_bytes,//amount of actually read bytes
			NULL//for synchronous operations
		);
		if (!writeFile_result) {
			throw SetPipeError("Failed to write to named pipe", GetLastError());
		}
		if (written_bytes != bytes_to_write) {
			throw SetPipeError("WriteFile failed. Written bytes amount is different from planned", GetLastError());
		}
		std::cout << "--Written to named pipe: " << write_buffer << std::endl;

		char read_buffer[128];
		DWORD bytes_to_read = (DWORD)sizeof(read_buffer);
		DWORD bytes_read = 0;

		BOOL readFile_result = ReadFile(
			hPipe,//pipe handle
			read_buffer,//out buffer
			bytes_to_read,//number of bytes to read
			&bytes_read,//number of actually read bytes
			NULL//for synchronous operations
		);

		if (!readFile_result) {
			throw SetPipeError("Failed to read from named pipe", GetLastError());
		}

		int to_copy = (bytes_read < sizeof(read_buffer)) ? bytes_read : (sizeof(read_buffer) - 1);
		read_buffer[to_copy] = '\0';
		std::cout << "--Read from named pipe: " << read_buffer << std::endl;

		std::cout << "--Reading/Writing message sequence--" << std::endl;
		int messages_amount;
		std::cout << "Enter the amount of messages: ";
		std::cin >> messages_amount;

		//TO DO: finish sequential message handling 
		std::string iterative_message;
		for (int i = 0; i < messages_amount; i++) {
			if (i == 0) {
				iterative_message = "Hello from client " + std::to_string(i);
			}

			DWORD iterative_written = 0;
			DWORD iterative_to_write = (DWORD)sizeof(iterative_message);

			BOOL iterative_write_result = WriteFile(
				hPipe,
				iterative_message.c_str(),
				iterative_to_write,
				&iterative_written,
				NULL
			);

			if (!iterative_write_result) {
				throw SetPipeError("Failed to wwrite iterative message to a named pipe", GetLastError());
			}
			if (iterative_written != iterative_to_write) {
				throw SetPipeError("WriteFile failed. Bytes written was different from planned", GetLastError());
			}
			std::cout << "--Written to named pipe: " << iterative_message << std::endl;

			char iterative_read_buffer[128];
			DWORD iterative_to_read = (DWORD)sizeof(iterative_read_buffer);
			DWORD iterative_read = 0;

			BOOL iterative_read_result = ReadFile(
				hPipe,
				iterative_read_buffer,
				iterative_to_read,
				&iterative_read,
				NULL
			);

			if (!iterative_read_result) {

				throw SetPipeError("Failed to read iterative message form named pipe", GetLastError());
			}

			std::cout << "--Read from named pipe: " << iterative_read_buffer << std::endl;
			std::string buff(iterative_read_buffer);
			iterative_message = buff;

		}
		std::cout << "--Sequential message handling done" << std::endl;


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