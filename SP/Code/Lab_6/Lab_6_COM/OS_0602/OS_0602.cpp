#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
#include "sddl.h"

#define HTPATH "C:/ProgramData/delete/HT.ht"
#define WIDEPATH L"C:/ProgramData/delete/HT.ht"

#include "../OS15_LIB/OS15_LIB.h"

#ifdef _WIN64
#pragma comment(lib, "../x64/Debug/OS15_LIB.lib")
#else
#pragma comment(lib, "../Debug/OS15_LIB.lib")
#endif

std::string intToString(int number);
SECURITY_ATTRIBUTES getSecurityAttributes();
HANDLE createControlEvent(const wchar_t* baseName, const wchar_t*suffix);

int main(int argc, char* argv[]) {
	HANDLE hStopEvent = createControlEvent(WIDEPATH,L"-stopEvent");

	try {
		std::cout << "Initiaizing COM compoment" << std::endl;
		OS15_HANDLE h = OS15_LIB::Init();

		HT::HTHANDLE* handle = OS15_LIB::OS15_HTCOM::Open_HT(h, "HTUser01", "1111", HTPATH);
		if (handle) {
			std::cout << "Successfully opened!" << std::endl;
		}
		else {
			throw "Failed to open a storage";
		}

		while (WaitForSingleObject(hStopEvent, 0) == WAIT_TIMEOUT) {
			
			int numberKey = rand() % 50;
			std::string key = intToString(numberKey);
			
			HT::Element* element = OS15_LIB::OS15_HTCOM::ConstructInsertElement_HT(h, key.c_str(), key.length() + 1, "0", 2);
			if (OS15_LIB::OS15_HTCOM::Insert_HT(h, handle, element)) {
				std::cout << "Element inserted: KEY=" << key << "; PAYLOAD=" << "0" << std::endl;

			}
			else {
				std::cerr << "Insertion failed" << std::endl;
			}
			delete element;
			Sleep(1000);
		}

		OS15_LIB::OS15_HTCOM::Close_HT(h, handle);

		std::cout << "Deleting COM componend and unloading DLL if possible" << std::endl;
		OS15_LIB::Dispose(h);


	}
	catch (const char* message) {
		std::cerr << "Error message: " << message << std::endl;
	}
	catch (int code) {
		std::cerr << "Error code: " << code << std::endl;
	}


}

std::string intToString(int number)
{
	std::stringstream convert;
	convert << number;

	return convert.str();
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

HANDLE createControlEvent(const wchar_t* baseName,const wchar_t*suffix)
{
	std::wstring name = L"Global\\";

	name += baseName;

	name += suffix;

	SECURITY_ATTRIBUTES SA = getSecurityAttributes();

	HANDLE hEvent = CreateEvent(

		&SA,

		TRUE, // manual-reset

		FALSE, // initially not signaled

		name.c_str());

	return hEvent;
}