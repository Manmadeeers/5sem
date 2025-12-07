#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#pragma comment(lib,"WS2_32.lib")

std::string SetError(std::string message, int code) {
	return message + ". Error code: " + std::to_string(code);
}

int main(int argc, char* argv[]) {
	try {
		WSADATA WSD_pointer;
		int WSD_version = MAKEWORD(2, 0);

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetError("WSAStartup() failed", WSAGetLastError());
		}
		std::cout << "--WSAStartup() executed" << std::endl;

		SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);

		if (inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr) <= 0) {
			throw SetError("inet_pton() function failed", WSAGetLastError());
		}

		if (connect(clientSocket, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetError("connect() function failed", WSAGetLastError());
		}

		const char* message = "Hello from Client!\n";

		if (send(clientSocket, message, strlen(message), NULL) == SOCKET_ERROR) {
			throw SetError("send() function failed", WSAGetLastError());
		}
		else {
			std::cout << "Message sent to server" << std::endl;
		}

		char in_buffer[50];
		if (recv(clientSocket, in_buffer, sizeof(in_buffer), NULL) == SOCKET_ERROR) {
			throw SetError("recv() function failed", WSAGetLastError());
		}
		else {
			std::cout << "Received from server: " << in_buffer << std::endl;
		}

		if (closesocket(clientSocket) == SOCKET_ERROR) {
			throw SetError("closesocket() function failed", WSAGetLastError());
		}
		std::cout << "Socket closed" << std::endl;

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetError("WSACleanup() function failed", WSAGetLastError());
		}
		std::cout << "Cleanup executed" << std::endl;


		system("pause");

	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}

	return 0;
}