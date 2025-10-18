#include <iostream>
#include <cstdlib>
#include "OS_11DLL.h"

#define STORAGE_NAME "Storage.ht"

using namespace std;


int main(int argc, char* argv[]) {
	if (argc != 6) {
		std::cerr << "Arguments: " << argc << std::endl;
		std::cerr << "Usage: <Capacity> <SnapshotInterval> <MaxKeyLength> <MaxPayloadLength>" << std::endl;
		return EXIT_FAILURE;
	}


	int ret_capacity = atoi(argv[2]);
	int ret_snapshot_interval = atoi(argv[3]);
	int ret_max_keylength = atoi(argv[4]);
	int ret_max_payloadlength = atoi(argv[5]);
	


	HT::HTHANDLE* storage = HT::Create(ret_capacity,ret_snapshot_interval,ret_max_keylength,ret_max_payloadlength,STORAGE_NAME);


	if (storage == NULL) {
		std::cerr << "Failed to create a storage. Look here for details: https://ru.stackoverflow.com" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "HT Storage Created. Filename: " << STORAGE_NAME << ", SecSnapshotInterval: " << ret_snapshot_interval << ", capacity: " 
		<< ret_capacity << ", maxkeylength: " << ret_max_keylength << ", maxpayloadlength: " << ret_max_payloadlength << std::endl;


	return EXIT_SUCCESS;
	system("pause");
}