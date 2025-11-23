#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ErrorHandler.h"
#pragma comment(lib, "Ws2_32.lib")
#define DESIRED_PORT 2000

bool GetServerByName(const char* name, const char* callsign, struct sockaddr* from, int* flen, SOCKET socket) {

	if (name == nullptr || callsign == nullptr || from == nullptr || flen == nullptr) {
		throw SetErrorMsgText("GetServerByName: invalid arguments", WSAEINVAL);
	}

	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	struct addrinfo* result = nullptr;
	int rc = getaddrinfo(name, nullptr, &hints, &result);
	if (rc != 0) {
		std::string msg = "GetServerByName: getaddrinfo failed: ";
		msg += gai_strerrorA(rc);
		throw SetErrorMsgText(msg, WSAHOST_NOT_FOUND);
	}

	sockaddr_in addr;
	ZeroMemory(&addr, sizeof(addr));
	bool found = false;
	for (struct addrinfo* i = result; i != nullptr; i = i->ai_next) {
		if (i->ai_addr && i->ai_addr->sa_family == AF_INET && i->ai_addrlen >= (int)sizeof(sockaddr_in)) {
			memcpy(&addr, i->ai_addr, sizeof(sockaddr_in));
			found = true;
			break;
		}
	}

	freeaddrinfo(result);

	if (!found) {
		throw SetErrorMsgText("GetServerByName: no IP found by required hostname", WSAHOST_NOT_FOUND);
	}

	addr.sin_port = htons(DESIRED_PORT);

	if (from != nullptr && flen != nullptr && *flen >= (int)sizeof(sockaddr_in)) {
		memcpy(from, &addr, sizeof(sockaddr_in));
		*flen = sizeof(sockaddr_in);
	}

	int addr_len = sizeof(addr);
	int send_result = sendto(socket, callsign, (int)strlen(callsign) + 1, NULL, (sockaddr*)&addr, addr_len);
	if (send_result == SOCKET_ERROR) {
		throw SetErrorMsgText("GetServerByName: failed to send callsign message", WSAGetLastError());
	}
	std::cout << "--Message with server callsign sent" << std::endl;

	char buffer[256];
	sockaddr_in resp;
	int resp_len = sizeof(resp);
	int received_len = recvfrom(socket, buffer, (int)sizeof(buffer), NULL, (sockaddr*)&resp, &resp_len);
	if (received_len == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAETIMEDOUT) {
			return false;
		}
		throw SetErrorMsgText("GetServerByName: failed to receive message from server", err);
	}

	if (received_len >= (int)sizeof(buffer)) {
		received_len = (int)sizeof(buffer) - 1;
	}
	buffer[received_len] = '\0';
	std::cout << "--Message received from server: " << buffer << std::endl;

	if (resp.sin_addr.S_un.S_addr != addr.sin_addr.S_un.S_addr) {
		return false;
	}

	if (strcmp(buffer, callsign) == 0) {
		if (*flen >= resp_len) {
			memcpy(from, &resp, resp_len);
			*flen = resp_len;
		}
		else {
			*flen = resp_len;
		}
		return true;
	}
	return false;
}



int main() {

	int WSD_version = MAKEWORD(2, 0);
	WSADATA WSD_pointer;

	try {

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Failed to startup: ", WSAGetLastError());
		}
		std::cout << "--Client started" << std::endl;

		SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (client_socket == INVALID_SOCKET) {
			throw SetErrorMsgText("Failed to create a socket: ", WSAGetLastError());
		}
		std::cout << "--Socket created" << std::endl;

		SOCKADDR_IN client;
		ZeroMemory(&client, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(0);
		client.sin_addr.S_un.S_addr = INADDR_ANY;

		int client_size = sizeof(client);
		if (bind(client_socket, (sockaddr*)&client, client_size) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to bind socket parameters: ", WSAGetLastError());
		}
		std::cout << "--Socket parameters binded" << std::endl;

		int optval = 1;
		if (setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to set socket options: ", WSAGetLastError());
		}
		std::cout << "--Socket options set to broadcast mode" << std::endl;

		const char* hostname = "Efrakiy";
		const char* server_callsign = "Hello";

		SOCKADDR_IN serv;
		ZeroMemory(&serv, sizeof(serv));
		int serv_len = sizeof(serv);

		bool found = GetServerByName(hostname, server_callsign, (sockaddr*)&serv, &serv_len, client_socket);
		if (!found) {
			std::cout << "Server with desired hostname and callsign not found" << std::endl;
		}
		else {
			std::cout << "Server with desired hostname and callsign found" << std::endl;
			char ipstr[INET_ADDRSTRLEN] = { 0 };
			inet_ntop(AF_INET, &serv.sin_addr, ipstr, sizeof(ipstr));
			std::cout << "Server IP: " << ipstr << ". Server PORT: " << ntohs(serv.sin_port) << std::endl;
		}

			

		if (closesocket(client_socket)==SOCKET_ERROR) {
			throw SetErrorMsgText("Field to close socket: ", WSAGetLastError());
		}

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup: ", WSAGetLastError());
		}
		std::cout << "--Cleanup executed" << std::endl;

		system("pause");

		return 0;

	}
	catch (string message) {
		std::cout << "Error: " << message << std::endl;
		system("pause");
	}
}