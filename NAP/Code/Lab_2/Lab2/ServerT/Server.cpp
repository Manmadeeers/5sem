#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstring>
#pragma comment(lib,"WS2_32.lib")
//#define GET_FIRST_HELLO
//#define GET_1000_MESSAGES
//#define GET_IP_AND_PORT
#define GET_AND_SEND

using namespace std;

string SetErrorMsgText(string text, int code);
string GetErrorMsgText(int code);



int main(int argc, _TCHAR* argv[]) {

	WSADATA WSD_pointer;
	int WSD_version = MAKEWORD(2, 0);

	SOCKET serverSocket;

	try {
		//--1: start the server
		if (WSAStartup(WSD_version, &WSD_pointer) != 0) {
			throw SetErrorMsgText("Startup: " , WSAGetLastError());
		}
		cout << "--Server started" << endl;

		//--2: create a server socket
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ;
		if (serverSocket == INVALID_SOCKET) {
			throw SetErrorMsgText("Socket Creation: ", WSAGetLastError());
		}

		cout << "--Server socket created" << endl;

		//socket parameters
		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.S_un.S_addr = INADDR_ANY;


		if (bind(serverSocket, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetErrorMsgText("Socket Parameter Binding: ", WSAGetLastError());
		}
		cout << "--Parameters binded" << endl;
		//--3: listen
		if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
			throw SetErrorMsgText("Listen: ", WSAGetLastError());
		}

		cout << "--Listening to a port" << endl;

#ifdef GET_IP_AND_PORT

		SOCKET clientSocket;
		SOCKADDR_IN client;
		memset(&client, 0, sizeof(client));
		int Lclient = sizeof(client);

		//--4: accept and wait
		cout << "--Accept and wait" << endl;
		clientSocket = accept(serverSocket, (sockaddr*)&client, &Lclient);
		if (clientSocket == INVALID_SOCKET) {
			throw SetErrorMsgText("Accept: ", WSAGetLastError());
		}

		char ip_buffer[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &client.sin_addr, ip_buffer, sizeof(ip_buffer)) == nullptr) {
			cerr << "Error converting IP address" << WSAGetLastError() << endl;
		}
		cout << "----------Client info----------" << endl;
		cout << "Client's IP address: " << ip_buffer << endl;
		cout << "Client's port: " << ntohs(client.sin_port) << endl;


#endif // GET_IP_AND_PORT


		


#ifdef GET_FIRST_HELLO
		//--5: recv and print
		cout << "----------Getting the first Hello----------" << endl;
		in_buffer_length = recv(clientSocket, in_buffer, sizeof(in_buffer) - 1, NULL);
		if (in_buffer_length == SOCKET_ERROR) {
			cerr << "Recv error: " << WSAGetLastError() << endl;
		}
		else {
			in_buffer[in_buffer_length] = '\0';
			complete_message += in_buffer;

			size_t pos;
			while ((pos = complete_message.find('\n') != string::npos)) {
				string msg = complete_message.substr(0, pos);
				cout << "Got from client: " << msg << endl;
				complete_message.erase(0, pos + 1);
			}


		}

		cout << "------------------------------------------------" << endl;

#endif // GET_FIRST_HELLO

#ifdef GET_1000_MESSAGES
		cout << "----------Getting a 1000 messages from client----------" << endl;
		while (true) {
			in_buffer_length = recv(clientSocket, in_buffer, sizeof(in_buffer) - 1, NULL);
			if (in_buffer_length == SOCKET_ERROR) {
				cerr << "Recv error: " << WSAGetLastError() << endl;
				break;
			}
			else if (in_buffer_length == 0) {
				cout << "Client disconnected" << endl;
				break;
			}
			else {
				in_buffer[in_buffer_length] = '\0';
				complete_message += in_buffer;

				size_t pos;
				while ((pos = complete_message.find('\n')) != string::npos) {
					string msg = complete_message.substr(0, pos);
					cout << "Got from client: " << msg << endl;
					complete_message.erase(0, pos + 1);
				}

				continue;
			}
		}

		cout << "----------------------------------------------------------" << endl;
#endif // GET_1000_MESSAGES

#ifdef GET_AND_SEND
		while (true) {
			SOCKET clientSocket;
			SOCKADDR_IN client;
			memset(&client, 0, sizeof(client));
			int Lclient = sizeof(client);

			clientSocket = accept(serverSocket, (sockaddr*)&client, &Lclient);

			if (clientSocket == INVALID_SOCKET) {
				cerr << "Failed to accept connection: " << WSAGetLastError() << endl;
				continue;
			}

			cout << "Client connected ("<<client.sin_port<<")" << endl;


			char in_buffer[50];
			int in_buffer_length = 0;

			string complete_message;


			while (true) {
				in_buffer_length = recv(clientSocket, in_buffer, sizeof(in_buffer), NULL);
				if (in_buffer_length == SOCKET_ERROR) {
					throw SetErrorMsgText("Failed to recv message from a client", WSAGetLastError());
					break;
				}
				else if (in_buffer_length <= 0) {
					cout << "Got the final message. Client Disconnected." << endl;
					break;
				}

				in_buffer[in_buffer_length] = '\0';

				cout << "Received from client: " << in_buffer << endl;
				if (send(clientSocket, in_buffer, sizeof(in_buffer), NULL) == SOCKET_ERROR) {
					throw SetErrorMsgText("Failed to send message bacl to client", WSAGetLastError());
				}
				else {
					cout << "Sent back to client: " << in_buffer << endl;
				}

			}

			if (closesocket(clientSocket) == SOCKET_ERROR) {
				throw SetErrorMsgText("Failed to close client socket", WSAGetLastError());
			}
			cout << "Client disconnected ("<<client.sin_port<<")" << endl;
		}

#endif // GET_AND_SEND
		
		system("pause");

		//--5: closure and cleanup
		//if (closesocket(clientSocket) == SOCKET_ERROR) {
		//	throw SetErrorMsgText("Client Socket closure", WSAGetLastError());
		//}
		//cout << "Client socket closed" << endl;

		if (closesocket(serverSocket) == SOCKET_ERROR) {
			throw SetErrorMsgText("Socket Closure: ", WSAGetLastError());
		}

		cout << "Server socket closed" << endl;

		if (WSACleanup() == SOCKET_ERROR) {
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
		}

		cout << "Cleanup executed" << endl;

		
	}
	catch (string error_msg) {
		cout << endl << error_msg << endl;
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