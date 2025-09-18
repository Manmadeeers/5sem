#include "Winsock2.h"
#include <tchar.h>
#include <iostream>
#include <cstring>
#pragma comment(lib,"WS2_32.lib")


using namespace std;


int main(int argc, _TCHAR* argv[]) {
	try {

	}
	catch (char* msg) {
		cout << msg << endl;
	}
	WSADATA WSD_pointer;
	int WSD_version = MAKEWORD(2, 0);
	if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
		throw "Unable to startup a client: " + WSAGetLastError();
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serv;

	serv.sin_family = AF_INET;
	serv.sin_port = htons(2000);
	//to be checked
	serv.sin_addr.S_un.S_addr = inet_addr("192.168.233.80");
	if (connect(clientSocket, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
		throw "Unable to connect: " + WSAGetLastError();;
	}


	return 0;
}