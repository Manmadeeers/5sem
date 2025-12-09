#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#pragma comment(lib,"WS2_32.lib")
#define PORT 40000
using namespace std;

std::string SetErrorMsgText(std::string message, int code) {
	return message + ". Error code: " + std::to_string(code);
}

int main(int argc, char* argv[]) {
	SOCKET sS;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup:", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(40000);
		serv.sin_addr.s_addr = inet_addr("127.0.0.1");

		if ((connect(sS, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
			throw SetErrorMsgText("connect:", WSAGetLastError());

		char ibuf[50];
		int  libuf = 0, lobuf = 0;

		string obuf = "Hello";

		if ((lobuf = send(sS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("send: ", WSAGetLastError());

		if ((libuf = recv(sS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)
			throw SetErrorMsgText("recv: ", WSAGetLastError());

		printf(ibuf);

		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}

	cout << endl;
	system("pause");
	return 0;
}