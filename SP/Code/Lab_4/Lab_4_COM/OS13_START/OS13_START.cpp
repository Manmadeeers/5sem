#pragma warning(disable : 4996)
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <atomic>

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
static std::atomic<bool>g_stopSnapshot(false);

void takeSnapshot(OS13_HANDLE h, HT::HTHANDLE* handle, int snapshot_interval) {

	while (!g_stopSnapshot.load()) {
		int total_ms = snapshot_interval * 1000;
		const int slice_ms = 200;
		for (int waited = 0; waited < total_ms && !g_stopSnapshot.load(); waited += slice_ms) {
			std::this_thread::sleep_for(std::chrono::milliseconds(slice_ms));
		}
		if (g_stopSnapshot.load()) {
			break;
		}
		if (handle) {
			OS13_LIB::OS13_HTCOM::Snap_HT(h, handle);
		}
	}
}


static unsigned int filename_hash(const char* str) {
	unsigned int hash = 5381;
	const unsigned char* p = reinterpret_cast<const unsigned char*>(str);

	while (*p) {
		hash = ((hash << 5) + hash) + *p;
		++p;
	}

	return hash;
}




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

	try {
		cout << "Initializing COM component:" << endl;
		OS13_HANDLE h = OS13_LIB::Init();

		char* fileName = argv[1];
		unsigned int hash = filename_hash(fileName);


		char availEventName[64];
		snprintf(availEventName, sizeof(availEventName), "Global\\HT_Available_%08X", hash);

		HANDLE hAvailEvent = CreateEventA(
			NULL,
			TRUE,
			FALSE,
			availEventName
		);
		if (!hAvailEvent) {
			throw "CreateEventA() for hAvailEvent failed. Error code: " + GetLastError();
		}


		HT::HTHANDLE* storage = OS13_LIB::OS13_HTCOM::Open_HT(h, fileName);
		if (!storage) {
			std::cerr << "Failed to open storage" << std::endl;
			if (hAvailEvent) {
				CloseHandle(hAvailEvent);
				hAvailEvent = NULL;
			}
			OS13_LIB::Dispose(h);
			return EXIT_FAILURE;
		}
		if (hAvailEvent) {
			if (!SetEvent(hAvailEvent)) {
				throw "SetEvent() for hAvailEvent failed. Error code: " + GetLastError();
			}
		}
		std::cout << "Storage opened" << std::endl;
		std::cout << "FileName: " << fileName << std::endl;
		std::cout << "Capacity: " << storage->Capacity << std::endl;
		std::cout << "SecSnapshotInterval: " << storage->SecSnapshotInterval << std::endl;
		std::cout << "MaxKeyLength: " << storage->MaxKeyLength << std::endl;
		std::cout << "MaxPayloadLength: " << storage->MaxPayloadLength << std::endl;

		std::thread snapshotThread(takeSnapshot,h, storage, storage->SecSnapshotInterval);

		while (!_kbhit()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		_getch();

		if (hAvailEvent) {
			if (!ResetEvent(hAvailEvent)) {
				throw "ResetEvent for hAvailEvent failed. Error code: " + GetLastError();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		g_stopSnapshot.store(true);

		if (snapshotThread.joinable()) {
			snapshotThread.join();
		}

		OS13_LIB::OS13_HTCOM::Snap_HT(h, storage);
		OS13_LIB::OS13_HTCOM::Close_HT(h, storage);
		storage = NULL;

		cout << endl << "Delete COM component and unload DLL if possible" << endl;
		OS13_LIB::Dispose(h);
		
		if (hAvailEvent) {
			CloseHandle(hAvailEvent);
			hAvailEvent = NULL;
		}
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}