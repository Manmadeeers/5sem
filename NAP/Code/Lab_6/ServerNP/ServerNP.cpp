#include <iostream>
#include <Windows.h>
#include <string>
#define PIPE_NAME "\\\\.\\pipe\\Tube"
//#define ITERATIVE_MESSAGES
#define TRANSACTIONS_AND_CALL

std::string SetPipeError(std::string message, int code) {
	return message+ ".Error: " + std::to_string(code);
}

std::string IncrementTailStringNumber(const std::string& s) {
	if (s.empty()) {
		return "1";
	}

	int i = (int)s.size() - 1;
	while (i >= 0 && std::isspace(static_cast<unsigned char>(s[i]))) {
		--i;
	}
	int endDigits = i;
	while (i >= 0 && std::isdigit(static_cast<unsigned char>(s[i]))) {
		--i;
	}
	int startDigits = i + 1;

	if (startDigits > endDigits) {
		return s + "1";
	}
	std::string prefix = s.substr(0, startDigits);
	std::string digits = s.substr(startDigits, endDigits - startDigits + 1);

	long long value = 0;

	try {
		value = std::stoll(digits);
	}
	catch (...) {
		return s + "1";
	}

	++value;
	return prefix + std::to_string(value);	

}

const std::string MESSAGE = "Hello from server";

int main(int argc, char* argv[]) {
	HANDLE hPipe;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;

	while (true) {
		try {
			hPipe = CreateNamedPipeA(
				PIPE_NAME,//named pipe name
				PIPE_ACCESS_DUPLEX,
				PIPE_TYPE_MESSAGE | PIPE_WAIT,//pipe access mode
				PIPE_UNLIMITED_INSTANCES,//max pipe instances
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

			BOOL connected = ConnectNamedPipe(
				hPipe,//named pipe handle
				NULL//NULL for synchronous connection
			);
			if (!connected) {
				DWORD err = GetLastError();
				if (err == ERROR_PIPE_CONNECTED) {
					std::cout << "--Client already connected (ERROR_PIPE_CONNECTED)" << std::endl;
				}
				else {
					throw SetPipeError("ConnectNamedPipe failed", err);
				}
				
			}
			std::cout << "--Named Pipe connected" << std::endl;

#ifdef TRANSACTIONS_AND_CALL

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

			std::string transact_read;
			char buffer[128];
			DWORD tbytes_read = 0;

			while (true) {
				BOOL read_result = ReadFile(
					hPipe,
					buffer,
					(DWORD)sizeof(buffer),
					&tbytes_read,
					NULL
				);
				if (!read_result) {
					DWORD err = GetLastError();
					if (err == ERROR_MORE_DATA) {
						transact_read.append(buffer,tbytes_read);
						continue;
					}
					else if (err == ERROR_BROKEN_PIPE) {
						throw SetPipeError("Client disconnected during ReadFile", err);
					}
					else {
						throw SetPipeError("ReadFile failed", err);
					}
				
				}
				if (tbytes_read > 0) {
					transact_read.append(buffer, tbytes_read);
				}
				break;
			}

			std::cout << "--Read from transaction: " << transact_read << std::endl;
			std::string transaction_reply = "Server transction reply";

			DWORD total_to_write = (DWORD)transaction_reply.size();
			DWORD total_written = 0;

			while (total_written < total_to_write) {
				DWORD iter_written = 0;

				BOOL write_result = WriteFile(
					hPipe,
					transaction_reply.c_str(),
					total_to_write,
					&iter_written,
					NULL
				);
				if (!write_result) {
					DWORD err = GetLastError();
					if (err == ERROR_BROKEN_PIPE) {
						throw SetPipeError("Client disconnected during WriteFile", err);
					}
					else {
						throw SetPipeError("WriteFile failed", err);
					}
				}

				if (iter_written == 0) {
					break;
				}
				total_written += iter_written;
			}
#endif // TRANSACTIONS


#ifdef ITERATIVE_MESSAGES

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
			strcpy_s(write_buffer, sizeof(write_buffer), MESSAGE.c_str());
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

			while (true) {
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
					DWORD err = GetLastError();
					if (err == ERROR_BROKEN_PIPE) {
						std::cout << "--Read/Write sequence ended. Exiting..." << std::endl;
						break;
					}
					throw SetPipeError("Failed to read iterative message from named pipe", err);

				}
				std::cout << "--Read from named pipe: " << iterative_read_buffer << std::endl;

				std::string s(iterative_read_buffer);
				std::string server_message = IncrementTailStringNumber(s);

				DWORD iterative_to_write = (DWORD)sizeof(server_message);
				DWORD iterative_written = 0;

				BOOL iterative_write_result = WriteFile(
					hPipe,
					server_message.c_str(),
					iterative_to_write,
					&iterative_written,
					NULL
				);
				if (!iterative_write_result) {
					throw SetPipeError("Failed to write iterative message to named pipe", GetLastError());
				}
				if (iterative_written != iterative_to_write) {
					throw SetPipeError("WriteFile failed for iterative message", GetLastError());
				}
				std::cout << "Written to named pipe: " << server_message << std::endl;

			}

#endif // ITERATIVE_MESSAGES

			if (DisconnectNamedPipe(hPipe) == 0) {
				throw SetPipeError("Failed to disconnect named pipe", GetLastError());
			}
			std::cout << "--Named pipe disconnected" << std::endl;

			if (CloseHandle(hPipe) == 0) {
				throw SetPipeError("Failed to close named pipe handle", GetLastError());
			}
			std::cout << "--Named pipe handle closed" << std::endl;

		}
		catch (std::string message) {
			std::cerr << message << std::endl;
		}
	}

	system("pause");

	return 0;
}
