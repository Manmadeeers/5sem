#include <winsock2.h>
#include <ws2tcpip.h>
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
		WSADATA WSD_pointer;
		int WSD_version = MAKEWORD(2, 0);

		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Failed to start", WSAGetLastError());
		}
		cout << "--client started" << endl;

		//2

		SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (clientSocket == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to create a client socket", WSAGetLastError());
		}

		cout << "--client socket created" << endl;

		//3
		SOCKADDR_IN serv;

		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		if (inet_pton(AF_INET, "10.210.39.148", &serv.sin_addr.S_un.S_addr) <= 0) {
			throw SetErrorMsgText("Failed to set an IP address", WSAGetLastError());
		}

		string outBuffer;
		int LoutBuffer = 0;

		outBuffer = "Hello from ClientU";
		LoutBuffer = sendto(clientSocket, outBuffer.c_str(), outBuffer.size() + 1, NULL, (sockaddr*)&serv, sizeof(serv));

		if (LoutBuffer == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to send message to server", WSAGetLastError());
		}

		cout << "--message sent (" << outBuffer << ")" << endl;
		
		//4
		char inBuffer[50];
		int LinBuffer = 0;
		SOCKADDR_IN inServ;
		int LinServ = sizeof(inServ);

		//Sleep(100);
		LinBuffer = recvfrom(clientSocket, inBuffer, sizeof(inBuffer) - 1, NULL, (sockaddr*)&serv, &LinServ);

		if (LinBuffer == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to receive message from server", WSAGetLastError());
		}

		inBuffer[LinBuffer] = '\0';

		cout << "--received from server (" << inBuffer << ")" << endl;

#ifdef SEND_AND_GET
		int amountMessages;
		cout << "Enter the amount of messages to be sent: ";
		cin >> amountMessages;

		string iNaBuffer;
		int LiNaBuffer = 0;

		for (int i = 0; i < amountMessages; i++) {
			outBuffer = "Hello from ClientU" + to_string(i);
			//Sleep(1500);
			LoutBuffer = sendto(clientSocket, outBuffer.c_str(), outBuffer.size() + 1, NULL, (sockaddr*)&serv, sizeof(serv));

			if (LoutBuffer == SOCKET_ERROR) {
				throw SetErrorMsgText("Failed to send message to server", WSAGetLastError());
			}

			cout << "--message sent (" << outBuffer << ")" << endl;



			/*LinBuffer = recvfrom(clientSocket, inBuffer, sizeof(inBuffer) - 1, NULL, (sockaddr*)&serv, &LinServ);

			if (LinBuffer == SOCKET_ERROR) {
				throw SetErrorMsgText("Failed to receive message from server", WSAGetLastError());
			}

			inBuffer[LinBuffer] = '\0';

			cout << "--received from server (" << inBuffer << ")" << endl;*/

		}

		if (sendto(clientSocket, "", 0, NULL, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to send the last message to the server", WSAGetLastError());
		}
		cout << "--last message sent to the server. disconnecting." << endl;
#endif // SEND_AND_GET


		//5
		if (closesocket(clientSocket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Failed to close client socket", WSAGetLastError());
		}
		cout << "--client socket closed" << endl;

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