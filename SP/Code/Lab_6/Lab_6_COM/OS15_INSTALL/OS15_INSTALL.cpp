#include <Windows.h>
#include <iostream>

#define SERVICENAME L"OS15_SERVICE"

#ifdef _WIN64
#define SERVICEPATH L"C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab_6\\Lab_6_COM\\x64\\Debug\\OS15_SERVICE.exe"
#else
#define SERVICEPATH L"C:\\Users\\Manmade\\Desktop\\5sem\\SP\\Code\\Lab_6\\Lab_6_COM\\x64\\Debug\\OS15_SERVICE.exe"
#endif

char* errortxt(const char* msg, int code)
{
	char* buf = new char[512];

	sprintf_s(buf, 512, "%s: Error code = %d\n", msg, code);

	return buf;
}


int main() {
	SC_HANDLE schService = NULL, schSCManager = NULL;
	try
	{
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

		if (!schSCManager)
		{
			throw errortxt("OpenSCManager", GetLastError());
		}
		else
		{
			schService = CreateService(
				schSCManager,
				SERVICENAME,
				SERVICENAME,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_SHARE_PROCESS,
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				SERVICEPATH,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
			);

			if (!schService)
			{
				throw errortxt("CreateService", GetLastError());
			}
		}
	}
	catch (char* txt)
	{
		std::cout << txt << std::endl;
	}


	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
	}

	if (schService)
	{
		CloseServiceHandle(schService);
	}

	return 0;
}

