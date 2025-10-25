#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "OS_11DLL.h"


static uint hash_function(const void* key, int keyLength) {
	int hash = 5381;
	const char* str = static_cast<const char*>(key);

	for (int i = 0; i < keyLength; ++i) {


		hash = ((hash << 5) + hash) + str[i];

	}

	std::cout << "--Hash: current Hash value: " << hash << std::endl;


	return static_cast<uint>(hash);
}



int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Arguments: " << argc << std::endl;
		std::cout << "Usage: ./os11_02.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	uint hash = hash_function(ret_filename, (int)strlen(ret_filename));
	
	char shutdown_event_name[64];
	snprintf(shutdown_event_name, sizeof(shutdown_event_name), "Global\\HT_Shutdown_%08X", hash);
	HANDLE hShutdownEvent = CreateEventA(
		NULL,//event attributes
		TRUE,//manual reset event
		FALSE,//initial state
		shutdown_event_name//event name
	);

	if (hShutdownEvent == NULL) {
		std::cerr << "WARNING: CreateEventA() failed. Error: " << GetLastError() << std::endl;
	}

	//TODO: finish with availability
	HT::HTHANDLE* storage = HT::Open(ret_filename);
	if (!storage) {

		DWORD err = GetLastError();
		std::cout << "Can't open a storage file. Error: " << err << std::endl;
		return EXIT_FAILURE;
	}


	srand((unsigned)time(NULL));

	while (true) {
		if (ShutdownEventHandle) {
			DWORD s = WaitForSingleObject(ShutdownEventHandle, 0);

			if (s == WAIT_OBJECT_0) {
				std::cout << "Shutdown event signaled. Exiting the loop" << std::endl;
				break;
			}
		}

		int key = rand() % 50;
		int value = rand() % 1000;

		HT::Element element(&key, sizeof(key), &value, sizeof(value));

		if (!HT::Insert(storage, &element)) {
			std::cout << "Insertion failed. Error: " << GetLastError() << std::endl;
		}
		else {
			std::cout << "Inserted: KEY=" << key << "; VALUE=" << value << ";" << std::endl;
		}

		if (ShutdownEventHandle) {
			DWORD waitRes = WaitForSingleObject(ShutdownEventHandle, 1000);
			if (waitRes==WAIT_OBJECT_0) {
				std::cout << "Shutdown event signaled during wait phase. Exiting the loop" << std::endl;
				break;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	if (ShutdownEventHandle) {
		CloseHandle(ShutdownEventHandle);
		ShutdownEventHandle = NULL;
	}

	HT::Snap(storage);
	HT::Close(storage);

	std::cout << "Worker exited clearly" << std::endl;

	return EXIT_SUCCESS;
}