#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

bool GetServer(char* call, struct sockaddr* from, int* flen,SOCKET*socket) {
	char buffer[50] = "";

	if (sendto(*socket, call, strlen(call) + 1, NULL, from, *flen) == SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to send message to server", WSAGetLastError());
	}
	cout << "--Message sent to server: " << call << endl;
	if (recvfrom(*socket, buffer, sizeof(buffer), NULL, from, flen) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT) {
			cout << "--Connection timed out" << endl;
			return false;
		}
		throw SetErrorMsgText("Failed to receive message from server", WSAGetLastError());
	}

	cout << "--Received message from server: " << buffer << endl;

	if (strcmp(buffer, call) == 0) {
		return true;
	}
	return false;
}

int _tmain(int argc, TCHAR* argv[]) {

	int WSD_version = MAKEWORD(2,0);
	WSADATA WSD_pointer;

	try {

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Failed to startup", WSAGetLastError());
		}
		cout << "--Client started" << endl;


		SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (client_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a socket", WSAGetLastError());
		}
		cout << "--Socket created" << endl;

		int optval = 1;

		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set socket options", WSAGetLastError());
		}
		cout << "--Socket options set to broadcast mode" << endl;

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_addr.S_un.S_addr = INADDR_BROADCAST;
		serv.sin_port = htons(2000);
		int serv_size = sizeof(serv);

		char out_buffer[50] = "Hello";

		if (GetServer(out_buffer, (sockaddr*)&serv, &serv_size, &client_socket)) {
			cout << "Got response from server!" << endl;
			cout << "Server IP: " << serv.sin_addr.S_un.S_addr << endl;
			cout << "Server port: " << serv.sin_port << endl;
		}
		else {
			cout << "Something went wrong" << endl;
		}

		
		if (closesocket(client_socket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close a socket", WSAGetLastError());
		}
		cout << "--Socket closed" << endl;


		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup", WSAGetLastError());
		}
		cout << "Cleanup executed" << endl;

	}
	catch (string message) {
		cerr << "Error: " << message << endl;
	}

	system("pause");
	return 0;
}