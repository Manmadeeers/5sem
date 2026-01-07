#include <Windows.h>
#include <iostream>


#define SERVICENAME L"OS15_SERVICE"


char* errortxt(const char* msg, int code)
{
	char* buf = new char[512];

	sprintf_s(buf, 512, "%s: error code = %d\n", msg, code);

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

		schService = OpenService(schSCManager, SERVICENAME, SERVICE_ALL_ACCESS);
		if (!schService)
		{
			throw errortxt("OpenService", GetLastError());
		}

		if (!DeleteService(schService))
		{
			throw errortxt("DeleteService", GetLastError());
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