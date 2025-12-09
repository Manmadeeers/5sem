#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#include <string>
#pragma comment(lib,"WS2_32.lib")
#define PORT 2000
using namespace std;

std::string SetErrorMsgText(std::string message, int code) {
	return message+" Error code: " + std::to_string(code);
}

int main(int argc, char*argv[]) {

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
		serv.sin_addr.s_addr = INADDR_ANY;

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
			throw SetErrorMsgText("bind:", WSAGetLastError());

		if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
			throw SetErrorMsgText("listen:", WSAGetLastError());

		while (true) {
			SOCKET cS;
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int lclnt = sizeof(clnt);

			if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET)
				throw SetErrorMsgText("accept:", WSAGetLastError());

			string obuf;
			bool flag = true;

			int lobuf = 0;
			int libuf = 0;
			char ibuf[50];


			if ((libuf = recv(cS, ibuf, sizeof(ibuf), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("recv: ", WSAGetLastError());

			obuf = "ECHO: " + (string)ibuf + '\0';
			if ((lobuf = send(cS, obuf.c_str(), strlen(obuf.c_str()) + 1, NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("send: ", WSAGetLastError());

			cout<<"Received from client: " << ibuf << endl;
		}

		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << "\nWSAGetLastError: " << errorMsgText;
	}
	cout << endl;
	return 0;
}