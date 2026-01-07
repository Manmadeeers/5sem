#include <iostream>
#include <Windows.h>
#include "../../Lab_6_COM/OS15_LIB/OS15_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS15_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS15_LIB.lib")
#endif 


int main(int argc, char* argv[]) {
	if (argc != 7) {
		std::cerr << "Usage: ./OS15_CREATE.exe <Capacity> <SecSnapshotInterval> <MaxKeyLength> <MaxPayloadLength> <UserGroup> <FileName>" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		std::cout << "Initialzing COM component" << std::endl;
		OS15_HANDLE h = OS15_LIB::Init();

		HT::HTHANDLE* handle = OS15_LIB::OS15_HTCOM::Create_HT(h, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5], argv[6]);
		if (handle) {
			std::cout << "HT storage created!" << std::endl;
			std::cout << "Stats: " << std::endl;
			std::cout << "Capacity: " << handle->Capacity << std::endl;
			std::cout << "SecSnapshotInterval: " << handle->SecSnapshotInterval << std::endl;
			std::cout << "MaxKeyLength: " << handle->MaxKeyLength << std::endl;
			std::cout << "MaxPayloadLength: " << handle->MaxPayloadLength << std::endl;
			std::cout << "UserGroup: " << handle->HTUserGroup << std::endl;
			std::cout << "FileName: " << handle->FileName << std::endl;

			if (OS15_LIB::OS15_HTCOM::Close_HT(h, handle)) {
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