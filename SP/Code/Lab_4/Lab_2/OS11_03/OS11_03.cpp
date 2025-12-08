//OS11_02 - for opening HT Storage alongside running OS11_START utility, generating random keys and attempting to delete elements with generated keys until aborted

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <string>
#include "OS_11DLL.h"

//#define DEBUG;

static uint filename_hash(const char* str) {

	uint hash = 5381;
	const unsigned char* p = reinterpret_cast<const unsigned char*>(str);

	while (*p) {
		hash = ((hash << 5) + hash) + *p; // djb2
		++p;
	}
	return hash;
}


int main(int argc, char*argv[]) {

	if (argc != 2) {
		std::cerr << "Arguments: " << argc << std::endl;
		std::cerr << "Usage: ./os11_03.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];
	uint hash = filename_hash(ret_filename);

	char avail_event_name[64];
	snprintf(avail_event_name, sizeof(avail_event_name), "Global\\HT_Available_%08X", hash);
	HANDLE hAvailEvent = NULL;

	HT::HTHANDLE* storage = nullptr;

	storage = HT::Open(ret_filename);

	if (!storage) {
		std::cout << "Initial HT Storage open failed. Error: " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Storage opened successfully" << std::endl;
	}



	srand((unsigned)time(NULL));

	while (true) {
		
		if (!storage) {
			if (!hAvailEvent) {
				hAvailEvent = OpenEventA(SYNCHRONIZE, FALSE, avail_event_name);

				if (!hAvailEvent) {
					std::this_thread::sleep_for(std::chrono::seconds(1));
					continue;
				}
				else {
					std::cout << "Availability event opened" << std::endl;
				}
			}

			DWORD w = WaitForSingleObject(hAvailEvent, INFINITE);

			if (w == WAIT_OBJECT_0) {
				storage = HT::Open(ret_filename);

				if (!storage) {
					std::cout << "Storage opening failed after availability event signal. Waiting for storage to be available again..." << std::endl;
					std::this_thread::sleep_for(std::chrono::seconds(1));
					continue;
				}
				else {
					std::cout << "Storage opened successfully after availability event signal" << std::endl;
				}
			}
			else if (w == WAIT_TIMEOUT) {
				std::cout << "WaitForSingleObject on availability event reached WAIT_TIMEOUT. Continuing" << std::endl;
				continue;
			}
			else {
				std::cerr << "WaitForSingleObject on availability event failed. Error: " << GetLastError() << std::endl;
				CloseHandle(hAvailEvent);
				hAvailEvent = NULL;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				continue;
			}


		}
		//if storage indicated as corupted
		if (!storage || storage->Addr == NULL) {

			if (storage) {
				std::cout << "WARNING: Invalid Addr detected. Closing storage handle and waiting for availability" << std::endl;
				HT::Close(storage);
				storage = nullptr;
			}
			continue;
		}
		if (WaitForSingleObject(hAvailEvent, 0) != WAIT_OBJECT_0) {
			std::cout << "Storage became unavailable before insert. Closing handle and wait..." << std::endl;

			if (storage) {

				HT::Close(storage);
				storage = nullptr;
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				continue;
			}
		}
		int rand_key = rand() % 50;
		std::string s = "key" + std::to_string(rand_key);
		const char* key = s.c_str();
		HT::Element*element = new HT::Element(key,(int)strlen(key));

		if (!HT::Delete(storage, element)) {
			DWORD err = GetLastError();
			if (err == 0) {
				std::cout << "Unable to delete an element with key " << key << ". Element with this key does not exist in a storage" << std::endl;
			}
			else {
				std::cout << "Deletion failed. Error: " << err << std::endl;
			}
			
		}
		else {
			std::cout << "Deleted an element with KEY=" << key << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (hAvailEvent) {
		CloseHandle(hAvailEvent);
		hAvailEvent = NULL;
	}

	HT::Snap(storage);
	HT::Close(storage);

	std::cout << "Worker (OS11_03) exited clearly" << std::endl;



	return EXIT_SUCCESS;
}