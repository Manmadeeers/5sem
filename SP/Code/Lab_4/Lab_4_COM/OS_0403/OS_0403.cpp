#include <iostream>
#include <Windows.h>
#include <string>
#include "../OS13_LIB/OS13_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS13_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS13_LIB.lib")
#endif 


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: ./OS_0403.exe <FileName>" << std::endl;
		return -1;
	}

	HANDLE hStopEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		L"Stop"
	);

	HANDLE hExitEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		L"Exit"
	);

	try {
		std::cout << "Initializing COM component" << std::endl;
		OS13_HANDLE h = OS13_LIB::Init();

		char* fileName = argv[1];
		HT::HTHANDLE* handle = OS13_LIB::OS13_HTCOM::Open_HT(h, fileName);
		if (handle) {
			std::cout << "Successfully opened" << std::endl;
		}
		else {
			std::cerr << "Failed to open" << std::endl;
			return -1;
		}

		while (WaitForSingleObject(hExitEvent, 0) == WAIT_TIMEOUT) {
			if (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT) {
				int tmpNum = rand() % 50;
				std::string s = "key" + std::to_string(tmpNum);
				const char* deleteKey = s.c_str();

				HT::Element* element = OS13_LIB::OS13_HTCOM::ConstructGetElement_HT(h, deleteKey, (int)strlen(deleteKey));

				if (OS13_LIB::OS13_HTCOM::Delete_HT(h,handle,element)) {
					std::cout << "Deleted an element with key" << s << std::endl;
				}
				else {
					std::cerr << "Failed to delete" << std::endl;
				}

				delete element;
				Sleep(1000);
			}
		}

	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}