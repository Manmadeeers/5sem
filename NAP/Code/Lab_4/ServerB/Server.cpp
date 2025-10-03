#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#include <string>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")

using namespace std;

bool GetRequestFromClient(SOCKET* socket,char* name, struct sockaddr* from, int* flen) {
	

	while (true) {
		char buffer[50];
		int buffer_length;

		buffer_length = recvfrom(*socket, buffer, sizeof(buffer), NULL, from, flen);
		if (buffer_length == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				return false;
			}
			else {
				throw SetErrorMsgText("Failed to receive a message", WSAGetLastError());
			}
		}

		if (strncmp(name, buffer, buffer_length) == 0) {
			return true;
		}
	}
}

bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto, SOCKET* socket) {
	if (sendto(*socket, name, strlen(name), NULL, to, *lto)==SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to put answer to client", WSAGetLastError());
	}
}


int _tmain(int arc, TCHAR* argv[]) {

	int WSD_version = MAKEWORD(2, 0);
	WSADATA WSD_pointer;
	char* server_callsign = (char*)"Hello";

	
	try {
		if (WSAStartup(WSD_version, &WSD_pointer)!=0) {
			throw SetErrorMsgText("Failed to startup", WSAGetLastError());
		}
		cout << "--Server started" << endl;

		
		SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (server_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a server socket", WSAGetLastError());
		}
		cout << "--Server socket created" << endl;

		SOCKADDR_IN serv;

		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.S_un.S_addr = INADDR_ANY;


		if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to bind server socket parameters", WSAGetLastError());
		}
		cout << "--Server socket parameters binded" << endl;
		
		SOCKET client_socket;
		SOCKADDR_IN client;
		memset(&client, 0, sizeof(client));
		int client_length = sizeof(client);

		char in_buffer[50] = "Hello";

		while (true) {
			if (GetRequestFromClient(&server_socket, server_callsign, (sockaddr*)&client, &client_length)) {
				PutAnswerToClient(in_buffer, (sockaddr*)&client, &client_length, &server_socket);
			}
		}

		if (closesocket(server_socket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close server socket", WSAGetLastError());
		}
		cout << "--Server socket closed" << endl;


		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup", WSAGetLastError());
		}
		cout << "--Cleanup executed" << endl;
		system("pause");
	}
	catch (string message) {
		cerr << "Error occured: " << message << endl;
	}

	return 0;
}