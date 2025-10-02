#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#pragma comment(lib,"WS2_32.lib")

using namespace std;

bool GetRequestFromClient(char* name, short port, struct sockaddr* from, int* flen) {

	SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server_socket == INVALID_SOCKET) {
		throw "Failed to create a server socket" + WSAGetLastError();
	}
	cout << "--Server socket created" << endl;

	SOCKADDR_IN serv;

	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.S_un.S_addr = INADDR_ANY;


	if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
		throw "Failed to bind server socket parameters" + WSAGetLastError();
	}
	cout << "--Server socket parameters binded" << endl;
	char buffer[256];

	while (true) {
		int rcv_length = recvfrom(server_socket, buffer, sizeof(buffer), NULL, (sockaddr*) & from, (int*) & flen);
		if (rcv_length == SOCKET_ERROR) {
			int last_error = WSAGetLastError();
			if (last_error == WSAETIMEDOUT) {
				return false;
			}
			else {
				throw "Failed to recieve a message" + WSAGetLastError();
			}
		}

		if (rcv_length < 0) {
			rcv_length = 0;
		}
		buffer[rcv_length] = '\0';

		if (rcv_length >= 0 && (int)strlen(name) && strncmp(buffer, name, strlen(name)) == 0) {
			from = 
		}
	}
}


int _tmain(int arc, TCHAR* argv[]) {

	int WSD_version = MAKEWORD(2, 0);
	WSADATA WSD_pointer;
	const char* server_callsign = "Hello";

	
	try {
		if (WSAStartup(WSD_version, &WSD_pointer)!=0) {
			throw "Failed to startup" + WSAGetLastError();
		}
		cout << "--Server started" << endl;

		

		SOCKET client_socket;
		SOCKADDR_IN client;
		memset(&client, 0, sizeof(client));
		int client_length = sizeof(client);

		char in_buffer[50];

		int Lin_buffer = 0;

		cout << "--Recv and wait" << endl;




		if (WSACleanup() == SOCKET_ERROR) {
			throw "Failed to cleanup" + WSAGetLastError();
		}
		cout << "--Cleanup executed" << endl;
		system("pause");
	}
	catch (string message) {
		cerr << "Error occured: " << message << endl;
	}

	return 0;
}