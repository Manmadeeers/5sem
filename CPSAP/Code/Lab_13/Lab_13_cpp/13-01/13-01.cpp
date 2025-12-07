#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#include <string>
#pragma comment(lib,"WS2_32.lib")

std::string SetError(std::string message, int code) {
	return message+" Error code: " + std::to_string(code);
}

int main(int argc, char*argv[]) {

	WSADATA WSD_pointer;
	int WSD_version = MAKEWORD(2, 0);

	SOCKET serverSocket;

	try {

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetError("Startup failed", WSAGetLastError());
		}
		std::cout << "--Server started" << std::endl;

		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket == INVALID_SOCKET) {
			throw SetError("socket() function failed", WSAGetLastError());
		}
		std::cout << "--Server socket created" << std::endl;

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.S_un.S_addr = INADDR_ANY;

		if (bind(serverSocket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetError("bind() function failed", WSAGetLastError());
		}
		std::cout << "--Socket parameters binded" << std::endl;

		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			throw SetError("listen() function failed", WSAGetLastError());
		}
		std::cout << "--Server listening to a port" << std::endl;

		char in_buffer[50];
		int in_buffer_length = 0;

		SOCKET clientSocket;
		SOCKADDR_IN client;
		memset(&client, 0, sizeof(client));
		int Lclient = sizeof(client);

		std::cout << "--Accept and wait" << std::endl;

		clientSocket = accept(serverSocket, (sockaddr*)&client, &Lclient);
		if (clientSocket == INVALID_SOCKET) {
			throw SetError("accept() function failed", WSAGetLastError());
		}

		char ip_buffer[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &client.sin_addr, ip_buffer, sizeof(ip_buffer)) == nullptr) {
			std::cerr << "Error converting IP address" << WSAGetLastError() << std::endl;
		}
		std::cout << "----------Client info----------" << std::endl;
		std::cout << "Client's IP address: " << ip_buffer << std::endl;
		std::cout << "Client's port: " << ntohs(client.sin_port) << std::endl;

		in_buffer_length = recv(clientSocket, in_buffer, sizeof(in_buffer) - 1, NULL);
		if (in_buffer_length == SOCKET_ERROR) {
			throw SetError("recv() function failed", WSAGetLastError());
		}
		else {
			in_buffer[in_buffer_length] = '\0';
			std::cout << "got from client: " << in_buffer << std::endl;
		}


		if (closesocket(serverSocket) == SOCKET_ERROR) {
			throw SetError("closesocket() function faield", WSAGetLastError());
		}

		std::cout << "--Server socket closed" << std::endl;

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetError("WSACleanup function failed", WSAGetLastError());
		}

		std::cout << "Cleanup executed" << std::endl;


		system("pause");

	
	}
	catch (std::string message) {
		std::cerr << message << std::endl;
	}

	return 0;
}