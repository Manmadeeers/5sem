#include <iostream>
#include <Windows.h>
#include <string>
#define PIPE_NAME "\\\\.\\pipe\\Tube"


std::string SetPipeError(std::string message, int code) {
	return message+ ".Error: " + std::to_string(code);
}

const std::string MESSAGE = "Hello from server";

int main(int argc, char* argv[]) {
	HANDLE hPipe;
	try {
		 hPipe= CreateNamedPipeA(
			PIPE_NAME,//named pipe name
			PIPE_ACCESS_DUPLEX, 
			PIPE_TYPE_MESSAGE | PIPE_WAIT,//pipe access mode
			1,//max pipe instances
			NULL,//out buffer size (optional, default:NULL)
			NULL,//in buffer size (optional, default:NULL)
			INFINITE,//timeout value for WaitNamedPipe (if NULL is transmitted then the parameter is set to default value of 50ms)
			NULL//security attributes (default value is NULL)
		);
		if (!hPipe) {
			DWORD err = GetLastError();
			if (hPipe == INVALID_HANDLE_VALUE) {
				throw SetPipeError("Failed to create named pipe handle", err);
			}
			else if (err == ERROR_INVALID_PARAMETER) {

				throw SetPipeError("nMaxInstances (Named pipe instances amount) parameter is greater then PIPE_UNLIMITED_INSTANCES", err);
			}
			else {
				throw SetPipeError("Something went wrong during named pipe creation", err);
			}
		}
		std::cout << "--Named pipe created" << std::endl;


		if (!ConnectNamedPipe(
			hPipe,//named pipe handle
			NULL//NULL for synchronous connection
		)) {
			throw SetPipeError("Failed to connect server program to a named pipe", GetLastError());
		}
		std::cout << "--Named Pipe connected" << std::endl;

		char read_buffer[128];
		DWORD read_bytes = 0;
		DWORD bytes_to_read = (DWORD)sizeof(read_buffer);

		BOOL readFile_result = ReadFile(
			hPipe,//pipe handle
			read_buffer,//out buffer
			bytes_to_read,//amount of bytes to read
			&read_bytes,//amount of actually read bytes
			NULL//for synchronous operations
		);

		if (!readFile_result) {
			throw SetPipeError("Failed to read info from named pipe", GetLastError());
		}
		int to_copy = (read_bytes < sizeof(read_buffer)) ? read_bytes : (sizeof(read_buffer) - 1);
		read_buffer[to_copy] = '\0';
		std::cout << "--Read from named pipe: " << read_buffer << std::endl;

		char write_buffer[128];
		strcpy_s(write_buffer, sizeof(write_buffer),MESSAGE.c_str());
		DWORD written_bytes = 0;
		DWORD bytes_to_write = (DWORD)sizeof(write_buffer);

		BOOL writeFile_result = WriteFile(
			hPipe,//pipe handle
			write_buffer,//out buffer
			bytes_to_write,//amount of bytes to write
			&written_bytes,//amount of actually written bytes
			NULL//for synchronous operations
		);
		if (!writeFile_result) {
			throw SetPipeError("Failed to write to a named pipe", GetLastError());
		}
		if (written_bytes != bytes_to_write) {
			throw SetPipeError("WriteFile failed. Amount of written bytes were different from planned", GetLastError());
		}
		std::cout << "--Written to named pipe: " << write_buffer << std::endl;

		std::cout << "--Writing/Reading message sequence from named pipe--" << std::endl;
		//TO DO: finish sequential message handling
		

		if (DisconnectNamedPipe(hPipe) == 0) {
			throw SetPipeError("Failed to disconnect named pipe", GetLastError());
		}
		std::cout << "--Named pipe disconnected" << std::endl;

		if (CloseHandle(hPipe)==0) {
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
