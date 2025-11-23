#include <iostream>
#include <string>
#include <Windows.h>
#define SLOT_NAME_LOCAL "\\\\.\\mailslot\\Box"
#define SLOT_NAME_NETWORK "\\\\DESKTOP-I\\mailslot\\Box"
#define SLOT_NAME_DOMAIN "\\\\*\\mailslot\\Box"

std::string SetMailslotError(std::string message, int code) {
	return "Error: " + message + "( " + std::to_string(code) + " )";
}


int main(int argc, char* argv[]) {
	HANDLE hMail;
	try {
		hMail = CreateFileA(
			SLOT_NAME_DOMAIN,//mailslot name
			GENERIC_WRITE,//desired access type
			FILE_SHARE_READ | FILE_SHARE_WRITE,//share mode
			NULL,//security attributes. NULL - for default
			OPEN_EXISTING,//mailslot opening flag
			FILE_ATTRIBUTE_NORMAL,//file attributes
			NULL//file template. NULL - for default
		);

		if (hMail == INVALID_HANDLE_VALUE) {
			throw SetMailslotError("Failed to open mailslot handle", GetLastError());
		}
		std::cout << "--Mailslot handle opened" << std::endl;

		char write_buffer[128] = "Hello from MailSlot";
		DWORD bytes_written = 0;

		BOOL write_result = WriteFile(
			hMail,//mailslot handle
			write_buffer,//write buffer
			(DWORD)sizeof(write_buffer),//bytes to write
			&bytes_written,//actually written bytes
			NULL//for sync/async operations
		);

		if (!write_result) {
			throw SetMailslotError("WriteFile failed", GetLastError());
		}
		if (bytes_written != (DWORD)sizeof(write_buffer)) {
			throw SetMailslotError("Number of bytes written was different from buffer size",GetLastError());
		}
		std::cout << "--Written to mailslot: " << write_buffer << std::endl;


		if (CloseHandle(hMail) == 0) {
			throw SetMailslotError("Failed to close mailslot handle", GetLastError());
		}
		std::cout << "--Mailslot handle closed" << std::endl;
	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}

	system("pause");
	return 0;
}