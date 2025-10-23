#include <iostream>
#include <cstdlib>
#include <thread>
#include "OS_11DLL.h"

using namespace std;

void takeSnapshot(HT::HTHANDLE* handle, int snapshot_interval) {

	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(snapshot_interval));
		if (handle) {
			HT::Snap(handle);
			std::cout << "Snapshot taken at " << handle->lastsnaptime << std::endl;
		}
	}

}


int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Arguments passed: " << argc << std::endl;
		std::cerr << "Usage: ./os11_start.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	HT::HTHANDLE* storage = HT::Open(ret_filename);
	if (storage == NULL) {
		std::cerr << "Failed to open. File does not exist" << std::endl;
		return EXIT_FAILURE;
	}

	cout << "HT Storage Started filename: " << ret_filename << ", snapshot interval: " << storage->SecSnapshotInterval << ", capacity: " << storage->Capacity
		<< ", max key length: " << storage->MaxKeyLength << ", max payload length: " << storage->MaxPayloadLength << std::endl;


	std::thread snapshotThread(takeSnapshot, storage, storage->SecSnapshotInterval);

	std::cout << "Press any key to stop: ";
	std::cin.get();

	HT::Close(storage);

	snapshotThread.detach();

	return EXIT_SUCCESS;
}