#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <string>	
#pragma comment(lib,"WS2_32.lib")
#define DEFAULT_PORT_NUMBER 2000

HANDLE hAcceptServer;

DWORD WINAPI AcceptServer(LPVOID pPrm) {
	DWORD rc = 0;

	while (true) {
		std::cout << "AcceptServer" << std::endl;
		Sleep(5000);
	}

	return rc;
}


int main(int argc, char* argv[]) {

	DWORD acceptId = 0;
	hAcceptServer = CreateThread(
		NULL,//security parameters. NULL-default value
		NULL,//stack size. if NULL - default value of 1MB
		AcceptServer,//thread function
		nullptr,//pointer to the function parameter
		NULL,//if NULL - thread function will execute right after the thread is created. if the parameter set to CREATE_SUSPENDED - the thread will wait for ResumeThread
		&acceptId
	);
	


	system("pause");
	return 0;
}	

