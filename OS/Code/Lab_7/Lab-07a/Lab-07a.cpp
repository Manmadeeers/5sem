#include <windows.h>
#include <iostream>
int main()
{
	SYSTEMTIME utcTime, localTime;

	GetSystemTime(&utcTime);
	GetLocalTime(&localTime);

	FILETIME ftUtc, ftLocal;
	SystemTimeToFileTime(&utcTime, &ftUtc);
	SystemTimeToFileTime(&localTime, &ftLocal);
	ULARGE_INTEGER uliUtc, uliLocal;
	uliUtc.LowPart = ftUtc.dwLowDateTime;
	uliUtc.HighPart = ftUtc.dwHighDateTime;
	uliLocal.LowPart = ftLocal.dwLowDateTime;
	uliLocal.HighPart = ftLocal.dwHighDateTime;

	LONGLONG diff = (LONGLONG)(uliLocal.QuadPart - uliUtc.QuadPart);
	diff /= 10000000LL;

	int offsetHours = (int)(diff / 3600);

	std::cout << localTime.wYear << "-";

	if (localTime.wMonth < 10) std::cout << "0";
	std::cout << localTime.wMonth << "-";

	if (localTime.wDay < 10) std::cout << "0";
	std::cout << localTime.wDay << "T";

	if (localTime.wHour < 10) std::cout << "0";
	std::cout << localTime.wHour << ":";

	if (localTime.wMinute < 10) std::cout << "0";
	std::cout << localTime.wMinute << ":";

	if (localTime.wSecond < 10) std::cout << "0";
	std::cout << localTime.wSecond;

	if (offsetHours >= 0) std::cout << "+";
	std::cout << offsetHours;

	return 0;
}
