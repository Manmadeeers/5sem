#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

bool GetServer(char* call, short port, struct sockaddr* from, int* flen,SOCKET socket) {
	char buffer[50] = "";
	if (sendto(socket,call,strlen(call)+1,NULL,from,*flen)==SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to send a message", WSAGetLastError());
	}
	int rcv_length = recvfrom(socket, buffer, sizeof(buffer), NULL, from, flen);
	if (rcv_length == SOCKET_ERROR) {

		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT) {
			return false;
		}
		else {
			throw SetErrorMsgText("Failed to receive a message", WSAGetLastError());
		}
		
	}
	cout << "Got from server: " << buffer << endl;

	if (strncmp(call, buffer, strlen(buffer)) == 0) {
		return true;
	}

	return false;

}


int _tmain(int argc, TCHAR* argv[]) {

	try {

		WSADATA WSD_pointer;
		int WSD_version = MAKEWORD(2, 0);

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Failed to start", WSAGetLastError());
		}
		cout << "--Client started" << endl;

		SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (client_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a client socket", WSAGetLastError());
		}

		cout << "--Client socket created" << endl;
		int optval = 1;

		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set client socket options", WSAGetLastError());
		}

		cout << "--Socket options set" << endl;

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.S_un.S_addr = INADDR_BROADCAST;
		int Lserv = sizeof(serv);

		char out_buffer[50] = "Hello";
		char in_buffer[50] = "";

		int out_buffer_length;
		int in_buffer_length;

		GetServer(out_buffer,)


	}
	catch (string message) {
		cout << "Error: " << message << endl;
	}

	return 0;
}