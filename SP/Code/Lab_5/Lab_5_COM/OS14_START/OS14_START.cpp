#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "../../Lab_5_COM/OS14_LIB/OS14_LIB.h"

#ifdef _WIN64
#pragma comment(lib,"../x64/Debug/OS14_LIB.lib")
#else
#pragma comment(lib,"../Debug/OS14_LIB.lib")
#endif


static std::atomic<bool>g_stopSnapshot(false);

static HANDLE g_hStart = nullptr;
static HANDLE g_hStop = nullptr;
static HANDLE g_hSuspend = nullptr;
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
			OS14_LIB::OS14_HTCOM::Snap_HT(h, handle);
		}
	}
}




int main(int argc, char* argv[]) {

	const char* StartEventName = "Global\\Start_Event";
	const char* StopEventName = "Global\\Stop_Event";
	const char* SuspendEventName = "Global\\Suspend_Event";
	try {
		g_hStart = CreateEventA(
			nullptr,
			TRUE,
			FALSE,
			StartEventName
		);
		if (!g_hStart) {
			std::cerr << "CreateEventA() for hStart failed. Error code: " << GetLastError() << std::endl;
			return EXIT_FAILURE;
		}

		g_hStop = CreateEventA(
			nullptr,
			TRUE,
			FALSE,
			StopEventName
		);
		if (!g_hStop) {
			std::cerr << "CreateEventA() for hStop failed. Error code: " << GetLastError() << std::endl;
			return EXIT_FAILURE;
		}

		g_hSuspend = CreateEventA(
			nullptr,
			TRUE,
			FALSE,
			SuspendEventName
		);
		if (!g_hSuspend) {
			std::cerr << "CreateEventA() for hSuspend failed. Error code: " << GetLastError() << std::endl;
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


		SetEvent(g_hStart);
		std::cout << "Start event signaled. Workers may proceed" << std::endl;


		std::thread snapshotThread(takeSnapshot, h, handle, handle->SecSnapshotInterval);

		std::cout << "Press any key to exit..." << std::endl;
		while (!kbhit()) {
			if (WaitForSingleObject(g_hSuspend,0) == WAIT_TIMEOUT) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else {
				system("pause");
				ResetEvent(g_hSuspend);
			}
		}
		_getch();	

		SetEvent(g_hStop);
		std::cout << "Stop event signaled. Workers should close" << std::endl;


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

		if (g_hStart) {
			CloseHandle(g_hStart);
			g_hStart = nullptr;
		}
		if (g_hStop) {
			CloseHandle(g_hStop);
			g_hStop = nullptr;
		}
		if (g_hSuspend) {
			CloseHandle(g_hSuspend);
			g_hSuspend = nullptr;
		}
	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}
}