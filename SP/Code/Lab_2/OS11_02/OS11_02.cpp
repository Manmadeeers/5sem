#include <iostream>
#include <cstdlib>
#include "OS_11DLL.h"


static uint hash_function(const void* key, int keyLength) {
	int hash = 5381;
	const char* str = static_cast<const char*>(key);

	for (int i = 0; i < keyLength; ++i) {


		hash = ((hash << 5) + hash) + str[i];

	}

	std::cout << "--Hash: current Hash value: " << hash << std::endl;


	return hash;
}



int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Arguments: " << argc << std::endl;
		std::cout << "Usage: ./os11_02.exe <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	HT::HTHANDLE* storage = HT::Open(ret_filename);
	if (!storage) {

		DWORD err = GetLastError();
		std::cout << "Can't open a storage file. Error: " << err << std::endl;
		return EXIT_FAILURE;
	}

	uint hash = hash_function(ret_filename, (int)strlen(ret_filename));

	char eventNameBuf[64];

	snprintf(eventNameBuf, sizeof(eventNameBuf), "Global\\HT_Shutdown_%08X", hash);

	HANDLE ShutdownEventHandle = CreateEventA(NULL, TRUE, FALSE, eventNameBuf);



	return EXIT_SUCCESS;
}