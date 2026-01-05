#include <iostream>
#include <Windows.h>
#include <string>
#include "../../Lab_5_COM/OS14_LIB/OS14_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS14_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS14_LIB.lib")
#endif


int main(int argc, char* argv[]) {
	const char* StartEventName = "Global\\Start_Event";
	const char* StopEventName = "Global\\Stop_Event";
	const char* SuspendEventName = "Global\\Suspend_Event";

	if (argc < 2 || argc>4) {
		std::cerr << "Usage: ./OS_0502.exe <FileName> OR ./OS_0502.exe <UserName> <Password> <FileName>" << std::endl;
		return EXIT_FAILURE;
	}

	try {
		HANDLE hStart = nullptr;
		while (!hStart) {
			hStart = OpenEventA(SYNCHRONIZE, FALSE, StartEventName);
			if (!hStart) {
				Sleep(200);
			}
		}

		DWORD wait = WaitForSingleObject(hStart, INFINITE);
		if (wait != WAIT_OBJECT_0) {
			std::cerr << "WaitForSingleObject() for hStart failed" << std::endl;
			CloseHandle(hStart);
			return EXIT_FAILURE;
		}
		std::cout << "Start event signaled. Starting..." << std::endl;

		HANDLE hStop = nullptr;
		while (!hStop) {
			hStop = OpenEventA(SYNCHRONIZE, FALSE, StopEventName);
			if (!hStop) {
				Sleep(200);
			}
		}

		HANDLE hSuspend = OpenEventA(SYNCHRONIZE, FALSE, SuspendEventName);
		if (!hSuspend) {
			std::cerr << "OpenEventA() for hSuspend failed. Error code: " << GetLastError() << std::endl;
			return EXIT_FAILURE;
		}

		OS14_HANDLE h = OS14_LIB::Init();
		HT::HTHANDLE* handle = nullptr;

		if (argc == 2) {
			handle = OS14_LIB::OS14_HTCOM::Open_HT(h, argv[1]);
			if (!handle) {
				std::cerr << "Failed to open" << std::endl;
				OS14_LIB::Dispose(h);
				return EXIT_FAILURE;
			}
			else {
				std::cout << "Successfully opened" << std::endl;
			}
		}
		else if (argc == 4) {
			handle = OS14_LIB::OS14_HTCOM::Open_HT(h, argv[1], argv[2], argv[3]);
			if (!handle) {
				std::cerr << "Failed to open" << std::endl;
				OS14_LIB::Dispose(h);
				return EXIT_FAILURE;
			}
			else {
				std::cout << "Successfully opened" << std::endl;
			}
		}

		std::atomic<bool>keepRunning{ true };

		std::thread worker([&]() {

			while (keepRunning) {
				if (WaitForSingleObject(hSuspend, 0) == WAIT_TIMEOUT) {
					int rand_key = rand() % 50;
					std::string s = "key" + std::to_string(rand_key);
					const char* key = s.c_str();

					HT::Element* getElement = OS14_LIB::OS14_HTCOM::ConstructGetElement_HT(h, key, (int)strlen(key));

					HT::Element* retElement = OS14_LIB::OS14_HTCOM::Get_HT(h, handle, getElement);
					if (retElement == NULL) {
						DWORD err = GetLastError();
						if (err == 0 || err == 183) {
							std::cout << "Element does not exist in a storage" << std::endl;
						}
						else {
							std::cerr << "Get_HT() failed. Error code: " << err << std::endl;
						}
					}
					else {
						std::cout << "Retrieved: ";
						OS14_LIB::OS14_HTCOM::Print_HT(h, retElement);

						const void* tmp_payload = retElement->Payload;
						int tmp_plength = retElement->PayloadLength;
						std::string payloadStr(reinterpret_cast<const char*>(tmp_payload), tmp_plength);
						unsigned int retPayload = 0;

						try {
							retPayload = std::stoull(payloadStr);

						}
						catch (const std::exception& ex) {
							std::cerr << "Failed to parse payload as integer. Error specific: " << ex.what() << std::endl;
							return EXIT_FAILURE;
						}

						retPayload++;

						std::string upd = std::to_string(retPayload);
						const char* upd_payload = upd.c_str();

						if (!OS14_LIB::OS14_HTCOM::Update_HT(h, handle, retElement, upd_payload, (int)strlen(upd_payload))) {
							DWORD err = GetLastError();
							if (err != 0) {
								std::cerr << "Update() failed. Error: " << err << std::endl;
							}
						}
						else {
							std::cout << "Element updated. New Payload: " << upd_payload << std::endl;
						}
					}
				}


				Sleep(1000);

			}

		});


		WaitForSingleObject(hStop, INFINITE);
		std::cout << "Stop event signaled. Exiting the application...";
		keepRunning = false;
		worker.join();

		CloseHandle(hStart);
		CloseHandle(hStop);
		CloseHandle(hSuspend);

		return EXIT_SUCCESS;

	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}