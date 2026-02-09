#include "Server.h"

int main(int argc, char* argv[]) {
	try {

		if (argc > 1) {
			int tmp_TCP = atoi(argv[1]);
			if (tmp_TCP >= 0 || tmp_TCP <= 65535) {
				TCP_PORT = tmp_TCP;
			}
			else {
				std::cerr << "Invalid TCP port" << std::endl;
			}
		}
		std::cout << "Server's TCP port: " << TCP_PORT << std::endl;

		if (argc > 2) {
			int tmp_UDP = atoi(argv[2]);
			if (tmp_UDP >= 0 || tmp_UDP <= 65535) {
				UDP_PORT = tmp_UDP;
			}
			else {
				std::cerr << "Invalid UDP port" << std::endl;
			}
		}
		std::cout << "Server's UDP port: " << UDP_PORT << std::endl;

		if (argc > 3) {

		}

	}
	catch (...) {
		std::cerr << "Critical error occured in the server's main thread!" << std::endl;
	}

	std::cout << "Sever stopped working" << std::endl;
	system("pause");
	return EXIT_SUCCESS;
}