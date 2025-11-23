#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#include <string>
#include "ErrorHandler.h"
#pragma comment(lib,"WS2_32.lib")
#define SERVER_CALLSIGN (char*)"Hello"
#define DESIRED_PORT 2000
using namespace std;

SOCKET client_socket;
SOCKET server_socket;

bool SendCheckMessage(char*callsign,int port, sockaddr*from, int*flen) {

	SOCKADDR_IN servers;
	servers.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	servers.sin_family = AF_INET;
	servers.sin_port = DESIRED_PORT;
	
	from = (sockaddr*)&servers;

	int check_sendlength = sendto(client_socket, callsign, strlen(callsign) + 1, NULL, from, *flen);
	if (check_sendlength == SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to send a message to check other servers presense", WSAGetLastError());
	}

	char buffer[50];
	int check_recvlength = recvfrom(client_socket, buffer, sizeof(buffer), NULL, from, flen);
	if (check_recvlength == SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to receive a message from another server", WSAGetLastError());
	}
	buffer[strlen(callsign)] = '\0';

	cout << "Another server callsign: " << buffer << endl;
	return true;
}

bool GetRequestFromClient(char* name, struct sockaddr* from, int* flen) {
	
	cout << "Recvfrom and wait" << endl;
	char buffer[50];
	int buffer_length;

	while (true) {

		buffer_length = recvfrom(server_socket, buffer, sizeof(buffer), NULL, from, flen);
		if (buffer_length == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to receive message from a client", WSAGetLastError());
		}
		cout << "Received from client: " << buffer << endl;

		if (strcmp(name, buffer) == 0) {
			return true;
		}
	}
}

bool PutAnswerToClient(char* name, struct sockaddr* to, int* lto) {

	int answer_length = sendto(server_socket, name, strlen(name) + 1, NULL, to, *lto);
	if (answer_length == SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to put answer to client", WSAGetLastError());
		return false;
	}
	cout << "Send back to client: " << name << endl;
	return true;
}


int main(int argc, char* argv[]) {

	int WSD_version = MAKEWORD(2, 0);
	WSADATA WSD_pointer;
	char* server_callsign = (char*)"Hello";

	
	try {

		if (WSAStartup(WSD_version, &WSD_pointer)!=0) {
			throw SetErrorMsgText("Failed to startup", WSAGetLastError());
		}
		cout << "--Server started" << endl;

		
		client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (server_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a server socket", WSAGetLastError());
		}
		cout << "--Server socket created" << endl;
		DWORD timer = 10;
		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)timer, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set socket to broadcast mode(timer)", WSAGetLastError());
		}
		cout << "--Client socket options set to broadcast mode" << endl;
		
		int server_optval = 1;
		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)server_optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set socket to broadcast mode(optval)", WSAGetLastError());
		}
		cout << "--Client socket options set to broadcast mode" << endl;

		SOCKADDR_IN all_servers;
		int all_servers_len = sizeof(all_servers);

		if (SendCheckMessage(SERVER_CALLSIGN, DESIRED_PORT, (sockaddr*)&all_servers, &all_servers_len)) {
			cout << "There are several servers within a network with a callsign " << SERVER_CALLSIGN << endl;
		}
		else {
			cout << "There are just one server within a network with a callsign " << SERVER_CALLSIGN << endl;
		}

		if (closesocket(client_socket)==SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close client socket", WSAGetLastError());
		}
		cout << "--Client socket closed" << endl;

		server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (server_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a server socket", WSAGetLastError());
		}
		cout << "--Server socket created" << endl;

		SOCKADDR_IN serv;
		serv.sin_addr.S_un.S_addr = INADDR_ANY;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(DESIRED_PORT);

		BOOL optval_serv = TRUE;

		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)optval_serv, sizeof(optval_serv))==SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set server socket options", WSAGetLastError());
		}
		cout << "--Server socket options set to broadcast mode" << endl;

		if (bind(server_socket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to bind server socket parameters", WSAGetLastError());
		}
		cout << "--Server socket parameters binded" << endl;


		SOCKADDR_IN from;
		memset(&from, 0, sizeof(from));
		int from_length = sizeof(from);

		while (true) {
			if (!GetRequestFromClient(SERVER_CALLSIGN, (sockaddr*)&from, &from_length)) {
				throw SetErrorMsgText("GetRequestFromClient failed", WSAGetLastError());
			}

			if (!PutAnswerToClient(SERVER_CALLSIGN, (sockaddr*)&from, &from_length)) {
				throw SetErrorMsgText("PutAnswerToClient failed", WSAGetLastError());
			}
		}

		if (closesocket(server_socket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close server socket", WSAGetLastError());
		}
		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup", WSAGetLastError());
		}
		cout << "--Cleanup executed" << endl;

	}
	catch (string message) {
		cerr << "Error occured: " << message << endl;
	}

	system("pause");
	return 0;
}