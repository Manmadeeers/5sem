#include "OS15_SERVICE.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS15_LIB.lib")
#else
#pragma comment(lib, "../Debug/OS15_LIB.lib")
#endif

WCHAR ServiceName[] = SERVICENAME;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
SERVICE_STATUS serviceStatus;

HANDLE hStopEvent = createStopEvent(WIDEPATH);

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	char temp[121];

	serviceStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	serviceStatus.dwWin32ExitCode = 0;
	serviceStatus.dwServiceSpecificExitCode = 0;
	serviceStatus.dwCheckPoint = 0;
	serviceStatus.dwWaitHint = 0;

	if (!(hServiceStatusHandle = RegisterServiceCtrlHandler(ServiceName, ServiceHandler)))
	{
		sprintf_s(temp, "RegisterServiceCtrlHandler failed. Error code = %d\n", GetLastError());
		trace(temp);
	}
	else
	{
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		if (!SetServiceStatus(hServiceStatusHandle, &serviceStatus))
		{
			sprintf_s(temp, "SetServiceStatus failed. Error code = %d\n", GetLastError());
			trace(temp);
		}
		else
		{
			trace("Start service", std::ofstream::out);
			startService();
		}
	}
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	char temp[121];

	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
	{
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		serviceStatus.dwCheckPoint = 0;
		serviceStatus.dwWaitHint = 0;

		SetEvent(hStopEvent);
		trace("Stop service");

		break;
	}
	case SERVICE_CONTROL_PAUSE:
		serviceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		sprintf_s(temp, "Unrecognized opcode %d\n", fdwControl);
		trace(temp);
	};

	if (!SetServiceStatus(hServiceStatusHandle, &serviceStatus))
	{
		sprintf_s(temp, "SetServiceStatus failed. Error code = %d\n", GetLastError());
		trace(temp);
	}
}

void startService()
{
	setlocale(LC_ALL, "Russian");

	try
	{
		std::cout << "Initializing COM component" << std::endl;
		OS15_HANDLE h = OS15_LIB::Init();

		HT::HTHANDLE* ht = OS15_LIB::OS15_HTCOM::Open_HT(h, USERNAME, PASSWORD, HTPATH);

		if (ht)
		{
			trace("Storage Started!\n");
			trace("SecSnapshotInterval: ");
			trace(std::to_string(ht->SecSnapshotInterval).c_str());
			trace("\nCapacity: ");
			trace(std::to_string(ht->Capacity).c_str());

			while (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT)
				SleepEx(0, TRUE);

			OS15_LIB::OS15_HTCOM::Snap_HT(h, ht);
			OS15_LIB::OS15_HTCOM::Close_HT(h, ht);

			trace("Storage closed");
		}
		else
			trace("Failed to open a storage\n");

		std::cout << std::endl << "Deleting COM component and unloading DLL if possible" << std::endl;
		OS15_LIB::Dispose(h);
	}
	catch (const char* message) {
		std::cout << "Error message: "<<message << std::endl;
	}
	catch (int code) {
		std::cout << "Error code: " << code << std::endl;
	}
}

SECURITY_ATTRIBUTES getSecurityAttributes()
{
	const wchar_t* sdd = L"D:"
		L"(D;OICI;GA;;;BG)" //Deny guests
		L"(D;OICI;GA;;;AN)" //Deny anonymous
		L"(A;OICI;GA;;;AU)" //Allow read, write and execute for Users
		L"(A;OICI;GA;;;BA)"; //Allow all for Administrators
	SECURITY_ATTRIBUTES SA;
	ZeroMemory(&SA, sizeof(SA));
	SA.nLength = sizeof(SA);
	ConvertStringSecurityDescriptorToSecurityDescriptor(
		sdd,
		SDDL_REVISION_1,
		&SA.lpSecurityDescriptor,
		NULL);

	return SA;
}
HANDLE createStopEvent(const wchar_t* stopEventName)
{
	std::wstring closeEventName = L"Global\\"; closeEventName += stopEventName; closeEventName += L"-stopEvent";
	SECURITY_ATTRIBUTES SA = getSecurityAttributes();

	HANDLE hStopEvent = CreateEvent(
		&SA,
		TRUE, //FALSE - автоматический сброс; TRUE - ручной
		FALSE,
		closeEventName.c_str());

	return hStopEvent;
}

void trace(const char* msg, int r)
{
	std::ofstream out;

	out.open(TRACEPATH, r);
	out << msg << std::endl;

	out.close();
}
