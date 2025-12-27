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


static unsigned int filename_hash(const char* str) {

	unsigned int hash = 5381;
	const unsigned char* p = reinterpret_cast<const unsigned char*>(str);

	while (*p) {
		hash = ((hash << 5) + hash) + *p; // djb2
		++p;
	}
	return hash;
}


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: ./OS_0402.exe <FileName>" << std::endl;
		return 1;
	}


	try {
		std::cout << "Initializing COM component" << std::endl;
		OS13_HANDLE h = OS13_LIB::Init();

		char* fileName = argv[1];
		unsigned int hash = filename_hash(fileName);

		char availEventName[64];
		snprintf(availEventName, sizeof(availEventName), "Global\\HT_Available_%08X", hash);
		HANDLE hAvailEvent = NULL;

		
		HT::HTHANDLE* storage = OS13_LIB::OS13_HTCOM::Open_HT(h, fileName);
		if (!storage) {
			throw "Open failed. Error code: " + GetLastError();
		}

		std::cout << "Successfully opened" << std::endl;

		srand((unsigned)time(NULL));
		while (true) {
			if (!storage) {
				if (!hAvailEvent) {
					hAvailEvent = OpenEventA(
						SYNCHRONIZE,
						FALSE,
						availEventName
					);
					if (!hAvailEvent) {
						throw "OpenEventA() for hAvailEvent failed. Error code: " + GetLastError();
					}
				}

				DWORD w = WaitForSingleObject(hAvailEvent, INFINITE);
				if (w == WAIT_OBJECT_0) {
					storage = OS13_LIB::OS13_HTCOM::Open_HT(h, fileName);
					if (!storage) {
						std::cout << "Storage is temporary unavailable. Waiting for storage to be available agail...";
						std::this_thread::sleep_for(std::chrono::seconds(1));
						continue;
					}
					else {
						std::cout << "Opened successfully" << std::endl;
					}
				}
				else if (w == WAIT_TIMEOUT) {
					std::cout << "Timeout reached. Continue" << std::endl;
					continue;
				}
				else {
					std::cerr << "WaitForSingleObject on availability event failed. Error code: " << GetLastError() << std::endl;

					CloseHandle(hAvailEvent);
					hAvailEvent = NULL;
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
			}
			if (!storage || storage->Addr == NULL) {
				if (storage) {
					std::cerr << "WARNING. Addr was not available. Waiting for storage to be available..." << std::endl;
					OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
					storage = nullptr;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
				}
				continue;
			}
			if (WaitForSingleObject(hAvailEvent, 0) != WAIT_OBJECT_0) {
				std::cout << "Storage became unavailable before insert. Closing handle ansd waiting for it to be available again" << std::endl;
				if (storage) {
					OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
					storage = nullptr;
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
			}

			int rand_key = rand() % 50;
			std::string s = "key" + std::to_string(rand_key);
			const char* key = s.c_str();
			std::string value = "0";

			HT::Element* insertElement = OS13_LIB::OS13_HTCOM::ConstructInsertElement_HT(h, key, (int)strlen(key), value.c_str(), (int)value.size());

			if (!OS13_LIB::OS13_HTCOM::Insert_HT(h, storage, insertElement)) {
				DWORD err = GetLastError();
				if (err != 0) {
					std::cerr << "Insertion failed. Error code: " << err << std::endl;
				}
				if (err == ERROR_INVALID_HANDLE || storage->Addr == NULL) {
					std::cerr << "Invalid storage indicated. Closing handle and waiting for availability." << std::endl;
					OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
					storage = nullptr;

					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					continue;
				}
			}
			else {
				std::cout << "Inserted: KEY=" << key << "; VALUE=" << value << std::endl;
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));

		}

		if (hAvailEvent) {
			CloseHandle(hAvailEvent);
			hAvailEvent = NULL;
		}
		if (storage) {
			OS13_LIB::OS13_HTCOM::Snap_HT(h, storage);
			OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
			storage = nullptr;
		}


		OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
		std::cout << "Delete COM component and unload DLL if possible" << std::endl;
		OS13_LIB::Dispose(h);

		std::cout << "Worker OS_0402 exited clearly" << std::endl;
		return EXIT_SUCCESS;
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}