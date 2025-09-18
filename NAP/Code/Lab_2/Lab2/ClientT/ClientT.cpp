#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
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

		if (inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr) <= 0) {
			throw "Invalid IP address: " + WSAGetLastError();
		}


		if (connect(clientSocket, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {//if server is not powered or actively refuses the connection - gives 10061

			throw "Unable to connect: " + WSAGetLastError();
		}


		char in_buffer[50];
		char out_buffer[50] = "server: accepted";

		int lin_buffer = 0;
		int lout_buffer = 0;

		if (lin_buffer = recv(clientSocket, in_buffer, sizeof(in_buffer), NULL) == SOCKET_ERROR) {
			throw "Recv: " + WSAGetLastError();
		}

		errno_t err = _itoa_s(lout_buffer, out_buffer + sizeof("Server: accepted") - 1, 10, 10);
		if (err != 0) {
			cerr << "Error converting a number via _itoa_s:" << err << endl;
		}
		if (lout_buffer = send(clientSocket, out_buffer, strlen(out_buffer) + 1, NULL) == SOCKET_ERROR) {
			throw "Send: " + WSAGetLastError();
		}

	}
	catch (char* msg) {
		cout << msg << endl;
	}
	
	return 0;
}