#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <iomanip>

int main() {

	_setmode(_fileno(stdout), _O_U16TEXT);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return 1;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	if (!Process32First(hSnapshot, &pe)) {
		return 1;
	}
	std::wcout << std::left << std::setw(30) << L"Name"
		<< std::setw(20) << L"PID"
		<< std::setw(20) << L"Parent PID" << L"\n";

	do {
		std::wcout << std::left << std::setw(30) << pe.szExeFile
			<< std::setw(20) << pe.th32ProcessID
			<< std::setw(20) << pe.th32ParentProcessID << L"\n";
	} while (Process32Next(hSnapshot, &pe));

	CloseHandle(hSnapshot);


	system("pause");
	return 0;
}