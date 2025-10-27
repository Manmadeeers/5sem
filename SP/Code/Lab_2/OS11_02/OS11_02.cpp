#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "OS_11DLL.h"


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
		std::cout << "Arguments: " << argc << std::endl;
		std::cout << "Usage: ./os11_02.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	uint hash = filename_hash(ret_filename);//will be the same as in OS11_START because FileName is the same
	
	char shutdown_event_name[64];
	snprintf(shutdown_event_name, sizeof(shutdown_event_name), "Global\\HT_Shutdown_%08X", hash);
	HANDLE hShutdownEvent = CreateEventA(
		NULL,//event attributes
		TRUE,//manual reset event
		FALSE,//initial state
		shutdown_event_name//event name
	);

	if (hShutdownEvent == NULL) {
		std::cerr << "WARNING: CreateEventA() for shutdown event failed. Error: " << GetLastError() << std::endl;
	}
	
	char avail_event_name[64];

	snprintf(avail_event_name, sizeof(avail_event_name), "Global\\HT_Available_%08X", hash);
	HANDLE hAvailabilityEvent = NULL;

	//trying to open HT_Available event even though it may not exist yet
	hAvailabilityEvent = OpenEventA(
		SYNCHRONIZE, //access mode
		FALSE, //inheritance status
		avail_event_name //event name
	);

	if (!hAvailabilityEvent) {
		std::cout << "Availability event named " << avail_event_name << " not found.Pending..." << std::endl;
	}
	else {
		std::cout << "Opened availability event named " << avail_event_name << std::endl;
	}

	HT::HTHANDLE* storage = HT::Open(ret_filename);
	if (!storage) {

		DWORD err = GetLastError();
		std::cout << "Initial HT Storage opening failed. Error: " << err << std::endl;

	}
	else {
		std::cout << "Storage opened successfully" << std::endl;
	}


	srand((unsigned)time(NULL));

	while (true) {

		//check for a shutdown request
		if (hShutdownEvent) {
			DWORD s = WaitForSingleObject(hShutdownEvent, 0);

			if (s == WAIT_OBJECT_0) {
				std::cout << "Shutdown event signaled. Exiting the loop" << std::endl;
				break;
			}
		}


		//ensure if the strage is available/open and if not - wait for an availability event
		if (!storage) {

			
			if (!hAvailabilityEvent) {

				hAvailabilityEvent = OpenEventA(SYNCHRONIZE, FALSE, avail_event_name);
				if (hAvailabilityEvent) {
					std::cout << "Availability event named " << avail_event_name << " opened." << std::endl;
				}

			}
			if(hAvailabilityEvent) {
				DWORD w = WaitForSingleObject(hAvailabilityEvent, 5000);
				if (w == WAIT_OBJECT_0) {
					storage = HT::Open(ret_filename);

					if (!storage) {
						DWORD err = GetLastError();
						std::cout << "Storage opening failed after availability event was signaled. Error: " << err << std::endl;

						std::this_thread::sleep_for(std::chrono::milliseconds(200));

						continue;

					}

					else {
						std::cout << "Storage successfully opened after availability event was signaled" << std::endl;
					}
				}
				else if (w == WAIT_TIMEOUT) {
					continue;
				}
				else {
					std::cerr << "WaitForSingleObject failed on availability event. Error: " << w << std::endl;
					std::cerr << "Will retry opening an availability event" << std::endl;

					CloseHandle(hAvailabilityEvent);
					hAvailabilityEvent = NULL;

					std::this_thread::sleep_for(std::chrono::seconds(1));

					continue;
				}
			}
			else {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				continue;
			}

		}

		int key = rand() % 50;
		int value = rand() % 1000;
		HT::Element element(&key, sizeof(key), &value, sizeof(value));

		if (storage->Addr==NULL) {
			std::cout << "Storage file mapping is not available." << std::endl;
			CloseHandle(storage);
			storage = NULL;
			continue;
		}

		if (!HT::Insert(storage, &element)) {
			DWORD err = GetLastError();

			std::cout << "Insertion failed. Error: " << err << std::endl;
			if (err == ERROR_INVALID_HANDLE || err == ERROR_INVALID_PARAMETER) {
				std::cout << "GetLastError() signaled a corrupted storage. Closing and waiting for re-opening." << std::endl;

				CloseHandle(storage);
				storage = NULL;

				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				continue;
			}
		}
		else {
			std::cout << "Inserted: KEY=" << key << "; VALUE=" << value << ";" << std::endl;
		}

		if (hShutdownEvent) {
			DWORD waitRes = WaitForSingleObject(hShutdownEvent, 1000);
			if (waitRes==WAIT_OBJECT_0) {
				std::cout << "Shutdown event signaled during wait phase. Exiting the loop" << std::endl;
				break;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	if (hShutdownEvent) {
		CloseHandle(hShutdownEvent);
		hShutdownEvent = NULL;
	}

	if (hAvailabilityEvent) {
		CloseHandle(hAvailabilityEvent);
		hAvailabilityEvent = NULL;
	}

	if (storage) {

		HT::Snap(storage);
		HT::Close(storage);

		storage = NULL;
	}

	std::cout << "Worker exited clearly" << std::endl;

	return EXIT_SUCCESS;
}