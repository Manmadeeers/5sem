#pragma warning(disable:4996)

#include <iostream>
#include <Windows.h>
#include <comdef.h>
#include "pch.h"
#include "OS13_LIB.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS13_LIB.lib")
#else
#pragma comment(lib, "../x64/Debug/OS13_LIB.lib")
#endif

int main(int argc, char* argv[]) {
	if (argc != 6) {
		std::cerr << "Usage: ./OS13_CREATE.exe <Capacity> <SecSnapshotInterval> <MaxKeyLength> <MaxPayloadLength>" << std::endl;
		return 1;
	}
	try {
		std::cout << "Initializing a component..." << std::endl;
		OS13_HTCOM_HAHNDLE h = OS13_LIB::Init();

		HT::HTHANDLE* handle = OS13_LIB::HT_LIB::LIB_Create(h, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
		if (handle) {
			std::cout << "LIB_Create: storage created" << std::endl;
			std::cout << "FileName: " << handle->FileName << std::endl;
			std::cout << "Capacity: " << handle->Capacity << std::endl;
			std::cout << "Snapshot Interval: " << handle->SecSnapshotInterval << std::endl;
			std::cout << "MaxKeyLength: " << handle->MaxKeyLength << std::endl;
			std::cout << "MaxPayloadLength: " << handle->MaxPayloadLength << std::endl;

			OS13_LIB::HT_LIB::LIB_Close(h, handle);
		}
		else {
			std::cerr << "LIB_Create: Failed to create an HT handle" << std::endl;
		}

		std::cout << "Disposing..." << std::endl;
		OS13_LIB::Dispose(h);
	}
	catch (const char* message) {
		std::cerr << "Error ocured. Message: " << message << std::endl;
	}
	catch (int code) {

		std::cout << "HRESULT error value: " << static_cast<HRESULT>(code) << std::endl;
	}
}