#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "ErrorHandler.h"

using namespace std;


bool GetServerByName(const char* name, const char* call, struct sockaddr* from, int* flen,SOCKET socket) {
	if (name == nullptr || call == nullptr) {
		throw SetErrorMsgText("GetServerByName: invalid arguments", WSAEINVAL);
	}

	HOSTENT* h = gethostbyname(name);
	if (h == nullptr) {
		throw SetErrorMsgText("gethostbyname failed", WSAGetLastError());
	}

	if (h->h_addr_list == nullptr || h->h_addr_list[0] == nullptr) {
		throw SetErrorMsgText("No address returned by gethostbyname", WSAHOST_NOT_FOUND);
	}


	SOCKADDR_IN addr;
	addr.sin_port = htons(2000);
	memcpy(&addr.sin_addr, h->h_addr_list[0], sizeof(addr.sin_addr));

	if (from != nullptr && flen != nullptr && *flen >= (int)sizeof(SOCKADDR_IN)) {
		memcpy(from, &addr, sizeof(SOCKADDR_IN));
		*flen = sizeof(SOCKADDR_IN);
	}


	int addr_len = sizeof(addr);
	int send_result = sendto(socket, call, (int)strlen(call) + 1, NULL, (sockaddr*)&addr, addr_len);
	if (send_result == SOCKET_ERROR) {
		throw SetErrorMsgText("Failed to send message with a callsign", WSAGetLastError());
	}
	cout << "--Message with server callsign sent" << endl;


	char buffer[256];
	SOCKADDR_IN resp;
	int resp_len = sizeof(resp);

	int received_len = recvfrom(socket, buffer, (int)sizeof(buffer), NULL, (sockaddr*)&resp, &resp_len);
	
	if (received_len == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT) {
			return false;
		}
		throw SetErrorMsgText("Failed to receive a message back", err);
	}
	buffer[received_len] = '\0';

	cout << "--Message received back: " << buffer << endl;

	if (resp.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) {
		return false;
	}

	if (strcmp(buffer, call) == 0) {
		return true;
	}
	return false;
}


int main() {

	int WSD_version = MAKEWORD(2, 0);
	WSADATA WSD_pointer;

	try {

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Failed to startup", WSAGetLastError());
		}
		cout << "--ClientS started" << endl;

		SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (client_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a socket", WSAGetLastError());
		}
		cout << "--Client socket created" << endl;


		SOCKADDR_IN client;
		client.sin_family = AF_INET;
		client.sin_port = htons(2000);
		client.sin_addr.S_un.S_addr = INADDR_ANY;

		int client_size = sizeof(client);


		if (bind(client_socket, (sockaddr*)&client, client_size) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to bind client socket parameters", WSAGetLastError());
		}

		cout << "--Client socket parameters binded" << endl;

		int optval = 1;

		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set socket options", WSAGetLastError());
		}
		cout << "--Socket options set to broadcast mode" << endl;

		const char* hostName = "localhost";
		const char* callSign = "Hello";

		SOCKADDR_IN serv;
		int serv_len = sizeof(serv);

		bool found = GetServerByName(hostName, callSign, (sockaddr*)&serv, &serv_len, client_socket);



		if (closesocket(client_socket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close client socket", WSAGetLastError());
		}
		cout << "Client socket closed" << endl;

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup", WSAGetLastError());
		}
		cout << "--Cleanup executed" << endl;

	}
	catch (string message) {

	}
	return 0;
}