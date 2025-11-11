#include <iostream>
#include <Windows.h>
#include <string>
#define SLOT_NAME_LOCAL "\\\\.\\mailslot\\Box"
#define THREE_MINUTES 180000

std::string SetMailslotError(std::string message, int code) {
	return "Error: " + message + "( " + std::to_string(code) + " )";
}


int main(int argc, char* argv[]) {
	HANDLE hMail;
	try {
		while (true) {
			hMail = CreateMailslotA(
				SLOT_NAME_LOCAL,//mailslot name
				(DWORD)300,//max message size. to specify that message could be of any size set to NULL
				THREE_MINUTES,//time a read operation can wait for message
				NULL//security attributes. set to NULL for default
			);

			if (hMail == INVALID_HANDLE_VALUE) {
				throw SetMailslotError("Failed to create Mailslot handle", GetLastError());
			}
			std::cout << "--Mailslot created" << std::endl;


			char read_buffer[128];
			DWORD read_bytes = 0;
			bool timeout_passed = false;

			BOOL read_result = ReadFile(
				hMail,//Mailslot handle
				read_buffer,//in buffer
				(DWORD)sizeof(read_buffer),//number of bytes to read
				&read_bytes,//number of actually read bytes
				NULL//for sync/async operations. NULL - for sync
			);

			if (!read_result) {
				DWORD err = GetLastError();
				if (err == 121) {
					std::cout << "--ReadFile timeout passed. Exiting..." << std::endl;
					timeout_passed = true;
				}
				else {
					throw SetMailslotError("ReadFile failed", err);
				}

			}
			if (!timeout_passed) {
				if (read_bytes < (DWORD)sizeof(read_buffer)) {
					read_buffer[(int)read_bytes] = '\0';
				}
				std::cout << "--Read from Mailslot: " << read_buffer << std::endl;
			}
			
			if (CloseHandle(hMail) == 0) {
				throw SetMailslotError("Failed to close Mailslot handle", GetLastError());
			}
			std::cout << "--Mailslot handle closed" << std::endl;
		}
		
	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}

	system("pause");
	return 0;
}