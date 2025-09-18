#include "Winsock2.h"
#include <tchar.h>
#include <iostream>
#include <cstring>
#pragma comment(lib,"WS2_32.lib")


using namespace std;


int main(int argc, _TCHAR* argv[]) {
	try {
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


		char in_buffer[50];
		char out_buffer[50] = "server: accepted";

		int lin_buffer = 0;
		int lout_buffer = 0;

		if (lin_buffer = recv(clientSocket, in_buffer, sizeof(in_buffer), NULL) == SOCKET_ERROR) {
			throw "Recv: " + WSAGetLastError();
		}

		_itoa(lout_buffer, out_buffer + sizeof("server: accepted") - 1, 10);
		if (lout_buffer = send(clientSocket, out_buffer, strlen(out_buffer) + 1, NULL) == SOCKET_ERROR) {
			throw "Send: " + WSAGetLastError();
		}

	}
	catch (char* msg) {
		cout << msg << endl;
	}
	
	return 0;
}