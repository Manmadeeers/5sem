#include <iostream>
#include <cstdlib>
#include <thread>
#include "OS_11DLL.h"

using namespace std;

static std::atomic<bool>g_stopSnapshot(false);

void takeSnapshot(HT::HTHANDLE* handle, int snapshot_interval) {

	while (!g_stopSnapshot.load()) {
		std::this_thread::sleep_for(std::chrono::seconds(snapshot_interval));
		if (g_stopSnapshot.load()) {
			break;
		}

		if (handle) {
			HT::Snap(handle);
		}
	}
}



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
		std::cerr << "Arguments passed: " << argc << std::endl;
		std::cerr << "Usage: ./os11_start.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	uint hash = filename_hash(ret_filename);
	char avail_event_name[64];
	snprintf(avail_event_name, sizeof(avail_event_name), "Global\\HT_Available_%08X", hash);

	HANDLE hAvailEvent = CreateEventA(
		NULL,//event attributes
		TRUE,//manual reset
		FALSE,//initial state
		avail_event_name//event name
	);

	if (hAvailEvent == NULL) {
		std::cerr << "WARNING: CreateEventA for Availability event failed. Error: " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Availability event handle created (name = " << avail_event_name << " )" << std::endl;
	}

	HT::HTHANDLE* storage = HT::Open(ret_filename);

	if (storage == NULL) {
		std::cerr << "Failed to open. File does not exist" << std::endl;
		//leaving availability event non signaled (so workers will wait) and exit
		if (hAvailEvent) {
			CloseHandle(hAvailEvent);
			hAvailEvent = NULL;
		}
		return EXIT_FAILURE;
	}
	

	//storage opened successfully, now signaling to workers that storage is available so they can proceed
	if (hAvailEvent) {
		if (!SetEvent(hAvailEvent)) {
			std::cerr << "WARNING: SetEvent failed. Error: " << GetLastError() << std::endl;
		}
		else {
			std::cout << "Availability event signaled. Storage available" << std::endl;
		}
	}

	cout << "HT Storage Started filename: " << ret_filename << ", snapshot interval: " << storage->SecSnapshotInterval << ", capacity: " << storage->Capacity
		<< ", max key length: " << storage->MaxKeyLength << ", max payload length: " << storage->MaxPayloadLength << std::endl;


	std::thread snapshotThread(takeSnapshot, storage, storage->SecSnapshotInterval);

	std::cout << "Press any key to stop: ";
	std::cin.get();

	//shutdown sequence below:

	if (hAvailEvent) {
		if (!ResetEvent(hAvailEvent)) {//clearing the availability event and notifying workers that storage is unavailable
			std::cerr << "WARNING: ResetEvent failed. Error: " << GetLastError() << std::endl;
		}
		else {
			std::cout << "Availability event reset. Storage now unavailable" << std::endl;	
		}
	}

	//requesting snapshot thread stop and join
	g_stopSnapshot.store(true);

	if (snapshotThread.joinable()) {
		snapshotThread.join();
	}

	//execute a snapshot and close storage
	HT::Snap(storage);
	HT::Close(storage);
	storage = NULL;

	//clean up event handle
	if (hAvailEvent) {
		CloseHandle(hAvailEvent);
		hAvailEvent = NULL;
	}

	std::cout << "OS11_START exited cleanly" << std::endl;

	return EXIT_SUCCESS;
}