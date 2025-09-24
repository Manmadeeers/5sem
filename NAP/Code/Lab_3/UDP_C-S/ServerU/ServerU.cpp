#include <winsock2.h>
#include <tchar.h>
#include <iostream>
#include <string>
#pragma comment(lib,"WS2_32.lib")
using namespace std;
#define SEND_AND_GET

string GetErrorMsgText(int code);
string SetErrorMsgText(string text, int code);

int main(int argc, _TCHAR* argv[]) {

	try {
		//1
		WSADATA WSA_pointer;
		int WSD_Version = MAKEWORD(2, 0);

		if (WSAStartup(WSD_Version, &WSA_pointer) != 0) {
			throw SetErrorMsgText("WSAStartup error", WSAGetLastError());
		}
		cout << "--server started" << endl;

		//2
		SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (serverSocket == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to create server socket", WSAGetLastError());
		}

		cout << "--server socket created" << endl;

		//3

		SOCKADDR_IN serverParameters;
		serverParameters.sin_family = AF_INET;
		serverParameters.sin_port = htons(2000);
		serverParameters.sin_addr.S_un.S_addr = INADDR_ANY;
		
		if (bind(serverSocket, (LPSOCKADDR)&serverParameters, sizeof(serverParameters)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to bind socket parameters", WSAGetLastError());
		}

		cout << "--parameters binded" << endl;

		SOCKET clientSocket;

		SOCKADDR_IN clientParameters;

		memset(&clientParameters, 0, sizeof(clientParameters));

		int LClient = sizeof(clientParameters);

		char inBuffer[50];

		int LinBuffer= 0;

		cout << "--recvfrom and wait" << endl;

		LinBuffer = recvfrom(serverSocket, inBuffer, sizeof(inBuffer), NULL, (sockaddr*)&clientParameters, &LClient);

		if (LinBuffer == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to receive message from client", WSAGetLastError());
		}
		inBuffer[LinBuffer] = '\0';

		cout << "--got from client (" << inBuffer << ")" << endl;

		//4

		cout << "--sending the message back to client" << endl;

		int sendResult = sendto(serverSocket, inBuffer, LinBuffer - 1, NULL, (sockaddr*)&clientParameters, LClient);
		if (sendResult == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to send message back to client", WSAGetLastError());
		}

		cout << "--sent back to client (" << inBuffer << ")" << endl;

#ifdef SEND_AND_GET
		
		while (true) {
			LinBuffer = recvfrom(serverSocket, inBuffer, sizeof(inBuffer), NULL, (sockaddr*)&clientParameters, &LClient);

			if (LinBuffer == SOCKET_ERROR) {
				throw SetErrorMsgText("Failed to receive message from client", WSAGetLastError());
			}
			if (LinBuffer == 0) {
				cout << "--last message was received. client disconnected." << endl;
				break;
			}
			inBuffer[LinBuffer] = '\0';

			cout << "--got from client (" << inBuffer << ")" << endl;

			cout << "--sending the message back to client" << endl;

			int sendResult = sendto(serverSocket, inBuffer, LinBuffer - 1, NULL, (sockaddr*)&clientParameters, LClient);
			if (sendResult == SOCKET_ERROR) {
				throw SetErrorMsgText("Failed to send message back to client", WSAGetLastError());
			}

			cout << "--sent back to client (" << inBuffer << ")" << endl;
		}

#endif // SEND_AND_GET


		//5

		if (closesocket(serverSocket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close server socket", WSAGetLastError());
		}

		cout << "--server socket closed" << endl;

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to cleanup", WSAGetLastError());
		}

		cout << "--cleanup executed" << endl;

		system("pause");


	}
	catch (string message) {
		cout << "An error occured: " << message << endl;
	}


	return 0;
}


string SetErrorMsgText(string text, int code) {
	char code_chr = static_cast<char>(code);
	return text + GetErrorMsgText(code) + "(" + code_chr + ")";
}

string GetErrorMsgText(int code) {

	string message;

	switch (code) {
	case WSAEINTR:
		message = "Function was stopped in the working process";
		break;
	case WSAEACCES:
		message = "Access denied";
		break;
	case WSAEFAULT:
		message = "Wring Address";
		break;
	case WSAEINVAL:
		message = "Argument Error";
		break;
	case WSAEMFILE:
		message = "Too many files opened";
		break;
	case WSAEWOULDBLOCK:
		message = "The resource is temporary unavailable";
		break;
	case WSAEINPROGRESS:
		message = "Operation is in development process";
		break;
	case WSAEALREADY:
		message = "Operation is already in execution process";
		break;
	case WSAENOTSOCK:
		message = "Socket set incorrectly";
		break;
	case WSAEDESTADDRREQ:
		message = "Address required";
		break;
	case WSAEMSGSIZE:
		message = "The message is too long";
		break;
	case WSAEPROTOTYPE:
		message = "Wring protocol type for a socket";
		break;
	case WSAENOPROTOOPT:
		message = "Error in a protocol option";
		break;
	case WSAEPROTONOSUPPORT:
		message = "Protocol is not supported";
		break;
	case WSAESOCKTNOSUPPORT:
		message = "Socket type is not supported";
		break;
	case WSAEOPNOTSUPP:
		message = "Operation is not supported";
		break;
	case WSAEPFNOSUPPORT:
		message = "Protocol type is not supported";
		break;
	case WSAEAFNOSUPPORT:
		message = "Address type is not supported";
		break;
	case WSAEADDRINUSE:
		message = "Address is already in use";
		break;
	case WSAEADDRNOTAVAIL:
		message = "Requested address could not be used";
		break;
	case WSAENETDOWN:
		message = "Network disconnected";
		break;
	case WSAENETUNREACH:
		message = "Could not reach the network";
		break;
	case WSAENETRESET:
		message = "The network shot down the connection";
		break;
	case WSAECONNABORTED:
		message = "Program denial of network";
		break;
	case WSAECONNRESET:
		message = "Connection restored";
		break;
	case WSAENOBUFS:
		message = "Not enough memory for buffers";
		break;
	case WSAEISCONN:
		message = "Socket is already connected";
		break;
	case WSAENOTCONN:
		message = "Socket disconnected";
		break;
	case WSAESHUTDOWN:
		message = "Could not execute send(): socket shut down";
		break;
	case WSAETIMEDOUT:
		message = "Connection timed out";
		break;
	case WSAECONNREFUSED:
		message = "Connection refused";
		break;
	case WSAEHOSTDOWN:
		message = "Host shut down";
		break;
	case WSAEHOSTUNREACH:
		message = "No route for host";
		break;
	case WSAEPROCLIM:
		message = "Too many processes";
		break;
	case WSASYSNOTREADY:
		message = "Network unreachable";
		break;
	case WSAVERNOTSUPPORTED:
		message = "This version is not supported";
		break;
	case WSANOTINITIALISED:
		message = "WS2_32.DLL was not initialized";
		break;
	case WSAEDISCON:
		message = "Shut down is in process";
		break;
	case WSATYPE_NOT_FOUND:
		message = "Type not found";
		break;
	case WSAHOST_NOT_FOUND:
		message = "Host not found";
		break;
	case WSATRY_AGAIN:
		message = "Unautharized host not found";
		break;
	case WSANO_RECOVERY:
		message = "Undefined error";
		break;
	case WSANO_DATA:
		message = "No record by a requiered type";
		break;
	case WSA_INVALID_HANDLE:
		message = "Descriptor came in with an error";
		break;
	case WSA_INVALID_PARAMETER:
		message = "One or more parameters came in with an error";
		break;
	case WSA_IO_INCOMPLETE:
		message = "I/O object is not is a signal state";
		break;
	case WSA_IO_PENDING:
		message = "Operation is to be completed lately";
		break;
	case WSA_NOT_ENOUGH_MEMORY:
		message = "Not enough memory";
		break;
	case WSA_OPERATION_ABORTED:
		message = "Operation aborted";
		break;
	case WSAEINVALIDPROCTABLE:
		message = "Invalid service";
		break;
	case WSAEINVALIDPROVIDER:
		message = "Invalid service version";
		break;
	case WSAEPROVIDERFAILEDINIT:
		message = "Failed to initialize a service";
		break;
	case WSASYSCALLFAILURE:
		message = "System call was aborted";
		break;

	}


	return message;
}