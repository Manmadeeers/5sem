#include <iostream>
#include <Windows.h>
#include "../../Lab_4_COM/OS13_LIB/OS13_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS13_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS13_LIB.lib")
#endif 

int main(int argc, char* argv[]) {
	if (argc != 6) {
		std::cerr << "Usage: ./OS13_CREATE.exe <Capacity> <SnapshptInterval> <MaxKeyLength> <MaxPayoadLength> <FileName>" << std::endl;
		return 1;
	}
	try {
		std::cout << "Initialzing COM component" << std::endl;
		OS13_HANDLE h = OS13_LIB::Init();

		HT::HTHANDLE* handle = OS13_LIB::OS13_HTCOM::Create_HT(h, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
		if (handle) {
			std::cout << "HT storage created!" << std::endl;
			std::cout << "Stats: " << std::endl;
			std::cout << "Capacity: " << handle->Capacity << std::endl;
			std::cout << "SecSnapshotInterval: " << handle->SecSnapshotInterval << std::endl;
			std::cout << "MaxKeyLength: " << handle->MaxKeyLength << std::endl;
			std::cout << "MaxPayloadLength: " << handle->MaxPayloadLength << std::endl;
			std::cout << "FileName: " << handle->FileName << std::endl;

			if (OS13_LIB::OS13_HTCOM::Close_HT(h, handle)) {
				std::cout << "Successfully closed after creation" << std::endl;
				return 0;
			}
			else {
				throw "Failed to close";
			}
		}
		else {
			std::cerr << "Create: Something went wrong" << std::endl;
		}
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}
