#include <iostream>
#include <cstdlib>

using namespace std;


int main(int argc, char* argv[]) {
	if (argc != 5) {
		std::cerr << "Usage: OS11_CREATE <Capacity> <SnapshotInterval> <MaxKeyLength> <MaxPayloadLength>" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}