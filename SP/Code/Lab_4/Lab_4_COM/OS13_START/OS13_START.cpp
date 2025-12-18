#pragma warning(disable : 4996)
#include <iostream>
#include <Windows.h>
#include <conio.h>

#include "../../Lab_4_COM/OS13_LIB/OS13_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS13_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS13_LIB.lib")
#endif

using namespace std;


OS13_HANDLE g_hHTCOM = nullptr;
HT::HTHANDLE* g_ht = nullptr;
HANDLE g_hExitEvent = nullptr;

BOOL WINAPI ConsoleHandler(DWORD signal) {
	if (signal == CTRL_CLOSE_EVENT || signal == CTRL_C_EVENT || signal == CTRL_BREAK_EVENT) {
		HANDLE hExitEvent = CreateEvent(NULL,
			TRUE, //FALSE - автоматический сброс; TRUE - ручной
			FALSE,
			L"Exit");
		SetEvent(hExitEvent);
	}
	return TRUE;
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cerr << "Usage: ./OS13_START.exe <FileName>" << std::endl;
		return 1;
	}

	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	HANDLE hStopEvent = CreateEvent(NULL,
		TRUE, //FALSE - автоматический сброс; TRUE - ручной
		FALSE,
		L"Stop");

	try {
		cout << "Initializing COM component:" << endl;
		OS13_HANDLE h = OS13_LIB::Init();

		char* fileName = argv[1];
		HT::HTHANDLE* ht = OS13_LIB::OS13_HTCOM::Open_HT(h, fileName);
		if (ht)
		{
			cout << "HT-Storage Started!" << endl;
			cout << "secSnapshotInterval: " << ht->SecSnapshotInterval << endl;
			cout << "capacity: " << ht->Capacity << endl;
			cout << "maxKeyLength: " << ht->MaxKeyLength << endl;
			cout << "maxPayloadLength: " << ht->MaxPayloadLength << endl;

			while (!kbhit())
				if (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT) {
					SleepEx(0, TRUE);
				}
				else {
					system("pause");
					ResetEvent(hStopEvent);
				}


			OS13_LIB::OS13_HTCOM::Snap_HT(h, ht);
			OS13_LIB::OS13_HTCOM::Close_HT(h, ht);
		}
		else
			cout << "-- open: error" << endl;

		cout << endl << "Delete COM component and unload DLL if possible" << endl;
		OS13_LIB::Dispose(h);
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}