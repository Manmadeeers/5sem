#include <iostream>
#include <cstdlib>
#include "OS_11DLL.h"

using namespace std;


int main(int argc, char* argv[]) {
	if (argc != 6) {
		std::cerr << "Arguments: " << argc << std::endl;
		std::cerr << "Usage: ./os11_create.exe <FileName> <Capacity> <SnapshotInterval> <MaxKeyLength> <MaxPayloadLength>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];
	int ret_capacity = atoi(argv[2]);
	int ret_snapshot_interval = atoi(argv[3]);
	int ret_max_keylength = atoi(argv[4]);
	int ret_max_payloadlength = atoi(argv[5]);
	


	HT::HTHANDLE* storage = HT::Create(ret_capacity,ret_snapshot_interval,ret_max_keylength,ret_max_payloadlength,ret_filename);

	if (storage == NULL) {
		std::cerr << "Failed to create a storage. Look here for details: https://ru.stackoverflow.com" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "HT Storage Created. Filename: " << ret_filename << ", SecSnapshotInterval: " << ret_snapshot_interval << ", capacity: " 
		<< ret_capacity << ", maxkeylength: " << ret_max_keylength << ", maxpayloadlength: " << ret_max_payloadlength << std::endl;


	return EXIT_SUCCESS;
	system("pause");
}