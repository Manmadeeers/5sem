#include <iostream>
#include <Windows.h>
#include "../../Lab_5_COM/OS14_LIB/OS14_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS14_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS14_LIB.lib")
#endif


int main(int argc, char* argv[]) {
	if (argc < 2||argc>4) {
		std::cerr << "Usage: ./OS_0502.exe <FileName> OR ./OS_0502.exe <UserName> <Password> <FileName>" << std::endl;
		return EXIT_FAILURE;
	}

	try {

		OS14_HANDLE h = OS14_LIB::Init();


	}
	catch (const char* message) {
		std::cerr << "Error message: "<<message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}