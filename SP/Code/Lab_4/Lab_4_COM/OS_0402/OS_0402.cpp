#pragma warning(disable:4996)

#include <iostream>
#include <Windows.h>
#include <string>
#include "../../Lab_4_COM/OS13_LIB/OS13_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS13_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS13_LIB.lib")
#endif 

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: ./OS_0402.exe <FileName>" << std::endl;
		return 1;
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

		char* filename = argv[1];
		HT::HTHANDLE* handle = OS13_LIB::OS13_HTCOM::Open_HT(h, filename);
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
				std::string s= "key" + std::to_string(tmpNum);
				const char* insertKey = s.c_str();
				std::string v = "0";

				HT::Element* element = new HT::Element(insertKey, (int)strlen(insertKey), v.c_str(), (int)v.size());
				if (OS13_LIB::OS13_HTCOM::Insert_HT(h, handle, element)) {
					std::cout << "Inserted element with key " << s << std::endl;
				}
				else {
					std::cerr << "Insert error" << std::endl;
				}

				delete element;
				Sleep(1000);
			}
		}

		OS13_LIB::OS13_HTCOM::Close_HT(h, handle);
		std::cout << "Delete COM component and unload DLL if possible" << std::endl;
		OS13_LIB::Dispose(h);
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}