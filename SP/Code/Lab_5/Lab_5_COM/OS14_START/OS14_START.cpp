#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <atomic>
#include "../../Lab_5_COM/OS14_LIB/OS14_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS14_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS14_LIB.lib")
#endif


static std::atomic<bool>g_stopSnapshot(false);

void takeSnapshot(OS14_HANDLE h, HT::HTHANDLE* handle, int snapshot_interval) {

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
			std::cout << "Snapshot executed" << std::endl;
			OS14_LIB::OS14_HTCOM::Snap_HT(h, handle);
		}
	}
}




int main(int argc, char* argv[]) {

	const char* StartEventName = "Global\\Start_Event";
	const char* StopEventName = "Global\\Stop_Event";
	try {
		HANDLE hStart = CreateEventA(
			nullptr,
			TRUE,
			FALSE,
			StartEventName
		);
		if (!hStart) {
			std::cerr << "CreateEventA() for hStart failed. Error code: " << GetLastError() << std::endl;
			return EXIT_FAILURE;
		}

		HANDLE hStop = CreateEventA(
			nullptr,
			TRUE,
			FALSE,
			StopEventName
		);
		if (!hStop) {
			std::cerr << "CreateEventA() for hStop failed. Error code: " << GetLastError() << std::endl;
			return EXIT_FAILURE;
		}

		std::cout << "Initializing COM component:" << std::endl;
		OS14_HANDLE h = OS14_LIB::Init();

		HT::HTHANDLE* handle;

		if (argc == 2) {
			handle = OS14_LIB::OS14_HTCOM::Open_HT(h, argv[1]);
			if (!handle) {
				std::cerr << "Failed to open a storage" << std::endl;
				OS14_LIB::Dispose(h);
				return EXIT_FAILURE;
			}
			else {
				std::cout << "Successfully opened!" << std::endl;
			}
		}
		else if (argc == 4) {
			handle = OS14_LIB::OS14_HTCOM::Open_HT(h, argv[1], argv[2], argv[3]);
			if (!handle) {
				std::cerr << "Failed to open a storage" << std::endl;
				OS14_LIB::Dispose(h);
				return EXIT_FAILURE;
			}
			else {
				std::cout << "Successfully opened!" << std::endl;
			}
		}
		else {
			std::cerr << "Usage: ./OS14_START.exe <FileName> OR ./OS14_START.exe <UserName> <Password> <FileName>" << std::endl;
			OS14_LIB::Dispose(h);
			return EXIT_FAILURE;
		}


		SetEvent(hStart);
		std::cout << "Start event signaled. Workers may proceed" << std::endl;


		std::thread snapshotThread(takeSnapshot, h, handle, handle->SecSnapshotInterval);

		std::cout << "Press any key to stop..." << std::endl;
		while (!_kbhit()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		_getch();


		SetEvent(hStop);
		std::cout << "Stop event signaled. Workers should close" << std::endl;

		if (hStart) {
			CloseHandle(hStart);
			hStart = NULL;
		}
		if (hStop) {
			CloseHandle(hStop);
			hStop = NULL;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		g_stopSnapshot.store(true);

		if (snapshotThread.joinable()) {
			snapshotThread.join();
		}

		if (handle) {
			OS14_LIB::OS14_HTCOM::Snap_HT(h, handle);
			OS14_LIB::OS14_HTCOM::Close_HT(h, handle);
			handle = NULL;
		}


		std::cout << std::endl << "Delete COM component and unload DLL if possible" << std::endl;
		OS14_LIB::Dispose(h);


	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}