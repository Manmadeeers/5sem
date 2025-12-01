#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <string>	
#include <conio.h>
#include "Helpers.h"
#pragma comment(lib,"WS2_32.lib")
#define DEFAULT_PORT_NUMBER 2000
#define ACCEPT_SERVER_START_COMMAND (LPVOID)"start"

HANDLE hAcceptServer;

DWORD WINAPI AcceptServer(LPVOID pPrm) {
	DWORD rc = 0;

	std::cout << "Transmitted parameter: " <<static_cast<const char*>(pPrm) << std::endl;

	return rc;
}


int main(int argc, char* argv[]) {
	int port_number = DEFAULT_PORT_NUMBER;
	bool port_set = false;
	switch (argc) {
	case 1:
		std::cout << "Server port(default): " << port_number<< std::endl;
		break;
	case 2:
		if (Helpers::parsePort(argv[1], port_number)) {
			std::cout << "Server port(from parameters): " << port_number << std::endl;
		}
		else {
			std::cout << "Server port(default. parameter was invalid): " << port_number;
		}
		break;
	default:
		std::cerr << "Usage: ./ConcurrentServer.exe <PortNumber>" << std::endl;
		break;
	}

	DWORD acceptId = 0;
	hAcceptServer = CreateThread(
		NULL,//security parameters. NULL-default value
		NULL,//stack size. if NULL - default value of 1MB
		AcceptServer,//thread function
		ACCEPT_SERVER_START_COMMAND,//pointer to the function parameter
		NULL,//if NULL - thread function will execute right after the thread is created. if the parameter set to CREATE_SUSPENDED - the thread will wait for ResumeThread
		&acceptId
	);

	if (hAcceptServer == nullptr) {
		std::cerr << "CreateThread for AcceptServer failed. Error code: "<<GetLastError() << std::endl;
		return 1;
	}

	std::cout << "AcceptServer thread created with PID=" << acceptId << std::endl;

	//TEMPORARY FOR FIRST TASK
	std::cout << "Press ENTER to stop the application" << std::endl;
	std::cin.get();
	//---
	
	if (!TerminateThread(hAcceptServer, 0)) {
		std::cerr << "TerminateThread for AcceptServer failed. Error code: " << GetLastError() << std::endl;

	}
	else {
		WaitForSingleObject(hAcceptServer, INFINITE);
	}
	
	CloseHandle(hAcceptServer);

	system("pause");
	return 0;
}	

