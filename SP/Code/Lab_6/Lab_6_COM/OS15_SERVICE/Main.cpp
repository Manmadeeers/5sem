#include "OS15_SERVICE.h"

int main() {
	WCHAR ServiceName[] = SERVICENAME;

	SERVICE_TABLE_ENTRY ServiceStartTable[] = {
		{ServiceName, ServiceMain},
		{NULL, NULL}
	};

	if (!StartServiceCtrlDispatcher(ServiceStartTable))
	{
		char temp[121];
		sprintf_s(temp, "StartServiceCtrlDispatcher failed. Error code = %d\n", GetLastError());
		trace(temp, std::ofstream::out);
	}

	return 0;

}