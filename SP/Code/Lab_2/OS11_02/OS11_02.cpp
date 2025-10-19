#include <iostream>
#include <cstdlib>
#include "OS_11DLL.h"


int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Arguments: " << argc << std::endl;
		std::cout << "Usage: <StorageFileName>" << std::endl;
		return EXIT_FAILURE;
	}

	char* ret_filename = argv[1];

	HT::HTHANDLE* storage = HT::Open(ret_filename);
	if (!storage) {
		std::cout << "Can't open. Storage file does not exist" << std::endl;
		return EXIT_FAILURE;
	}



	return EXIT_SUCCESS;
}