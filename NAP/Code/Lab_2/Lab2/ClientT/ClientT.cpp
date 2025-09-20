#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <string>
#pragma comment(lib,"WS2_32.lib")
//#define SEND_FIRST_HELLO
//#define SEND_1000_MESSAGES
#define SEND_AND_GET
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

			cerr << "Unable to connect: " << WSAGetLastError() << endl;
		}

#ifdef SEND_FIRST_HELLO
		const char* message = "Hello from Client!";

		if (send(clientSocket, message, strlen(message), NULL) == SOCKET_ERROR) {
			cerr << "Send error:" << WSAGetLastError() << endl;
		}
		else {
			cout << "Message sent to server" << endl;
		}

#endif // SEND_FIRST_HELLO


#ifdef SEND_1000_MESSAGES
		for (int i = 0; i < 1000; i++) {
			string message_xxx = "Hello from Client " + to_string(i) + "\n";
			if (send(clientSocket, message_xxx.c_str(), message_xxx.size(), NULL) == SOCKET_ERROR) {
				cerr << "Send error: " << WSAGetLastError() << endl;
				break;
			}
			else {
				cout << "Message sent: " << message_xxx << endl;
			}
		}

		if (shutdown(clientSocket, SD_BOTH) == SOCKET_ERROR) {
			cerr << "Failed to shutdown: " << WSAGetLastError() << endl;
		}
		else {
			cout << "Successfully shut down" << endl;
		}

#endif // SEND_1000_MESSAGES

#ifdef SEND_AND_GET

		int msg_amount;
		cout << "Enter the amount of messages to be transmitted: ";
		cin >> msg_amount;


		clock_t start_time = clock();
		string message;
		for (int i = 0; i < msg_amount; i++) {
			if (i == 0) {
				message = "Hello from client" + to_string(i) + "\n";
			}

			if (send(clientSocket, message.c_str(), message.size(), NULL) == SOCKET_ERROR) {
				cerr << "Failed to send a message: " << WSAGetLastError() << endl;
				break;
			}
			else {
				cout << "Message sent: " << message << endl;
			}

			char in_buffer[50];
			if (recv(clientSocket, in_buffer, sizeof(in_buffer), NULL) == SOCKET_ERROR) {
				cerr << "Failed to recv a message: " << WSAGetLastError() << endl;
				break;
			}
			else {
				cout << "Recieved from server: " << in_buffer << endl;
			}

			int iterator = i + 1;
			string tmp = "";
			message = "";
			tmp += in_buffer;
			message = "Hello from client" + to_string(iterator) + "\n";
		}


		string final_message = "";

		if (send(clientSocket, final_message.c_str(), final_message.size(), NULL) == SOCKET_ERROR) {
			cerr << "Failed to send the last message: " << WSAGetLastError() << endl;
		}
		else {
			cout << "Last message sent. Disconnecting" << endl;
		}

		clock_t end_time = clock();

		double time_duration = static_cast<double>(end_time - start_time);

		cout << "Time taken for " << msg_amount << " messages: " << time_duration << endl;

#endif // SEND_AND_GET



		system("pause");


		if (closesocket(clientSocket) == SOCKET_ERROR) {
			cerr << "Failed to close a socket"<<WSAGetLastError() << endl;
		}
		cout << "Socket closed" << endl;

		if (WSACleanup() == SOCKET_ERROR) {
			cerr << "Failed to cleanup" << WSAGetLastError() << endl;
		}
		cout << "Cleanup executed" << endl;


		system("pause");


	}
	catch (char* msg) {
		cout << msg << endl;
	}
	
	return 0;
}