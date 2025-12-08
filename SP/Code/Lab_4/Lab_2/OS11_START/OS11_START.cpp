#include <iostream>
#include <cstdlib>
#include <thread>
#include <conio.h>
#include "OS_11DLL.h"

using namespace std;

static std::atomic<bool>g_stopSnapshot(false);

void takeSnapshot(HT::HTHANDLE* handle, int snapshot_interval) {

	while (!g_stopSnapshot.load()) {
		int total_ms = snapshot_interval * 1000;
		const int slice_ms = 200;
		for (int waited = 0; waited < total_ms && !g_stopSnapshot.load(); waited += slice_ms) {
			std::this_thread::sleep_for(std::chrono::milliseconds(slice_ms));
		}
		if (g_stopSnapshot.load()) {
			break;
		}
		if (handle) {
			HT::Snap(handle);
		}
	}
}



static uint filename_hash(const char* str) {
	uint hash = 5381;
	const unsigned char* p = reinterpret_cast<const unsigned char*>(str);

	while (*p) {
		hash = ((hash << 5) + hash) + *p; 
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

	if (!hAvailEvent) {
		std::cerr << "WARNING: CreateEventA for Availability event failed. Error: " << GetLastError() << std::endl;
	}
	else {
		std::cout << "Availability event handle created (name = " << avail_event_name << " )" << std::endl;
	}


	HT::HTHANDLE* storage = HT::Open(ret_filename);

	if (!storage) {
		std::cerr << "Failed to open. File does not exist" << std::endl;
		if (hAvailEvent) {
			CloseHandle(hAvailEvent);
			hAvailEvent = NULL;
		}
		return EXIT_FAILURE;
	}

	if (hAvailEvent) {
		if (!SetEvent(hAvailEvent)) {
			std::cerr << "WARNING: SetEvent for Availability event failed. Error: " << GetLastError() << std::endl;
		}
		else {
			std::cout << "Availability event set. Storage available for workers" << std::endl;
		}
	}

	std::cout << "HT storage started" << std::endl;
	std::cout << "FileName: " << ret_filename << ", Capacity: " << storage->Capacity
		<< ", SnapshotInterval: " << storage->SecSnapshotInterval
		<< ", Max Key Length: " << storage->MaxKeyLength
		<< ", Max Payload Length: " << storage->MaxPayloadLength
		<< std::endl;


	std::thread snapshotThread(takeSnapshot, storage, storage->SecSnapshotInterval);

	std::cout << "Press any key to stop: ";

	while (!_kbhit()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	_getch();

	
	if (hAvailEvent) {
		if (!ResetEvent(hAvailEvent)) {
			std::cerr << "WARNING: ResetEvent for Availability event failed. Error: " << GetLastError() << std::endl;
		}
		else {
			std::cout << "Availability event reset. Workers should stop inserting and wait." << std::endl;
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	g_stopSnapshot.store(true);

	if (snapshotThread.joinable()) {
		snapshotThread.join();
	}

	HT::Snap(storage);
	HT::Close(storage);
	storage = NULL;


	if (hAvailEvent) {
		CloseHandle(hAvailEvent);
		hAvailEvent = NULL;
	}

	std::cout << "OS11_START exited cleanly" << std::endl;

	return EXIT_SUCCESS;
}