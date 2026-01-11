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
int charToInt(char* str);
std::string incrementPayload(char* str);
SECURITY_ATTRIBUTES getSecurityAttributes();
HANDLE createStopEvent(const wchar_t* stopEventName);

int main(int argc, char* argv[]) {
	HANDLE hStopEvent = createStopEvent(WIDEPATH);

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
			
			HT::Element* Constructedelement = OS15_LIB::OS15_HTCOM::ConstructGetElement_HT(h, key.c_str(), key.length() + 1);
			HT::Element* element = OS15_LIB::OS15_HTCOM::Get_HT(h, handle, Constructedelement);
			if (element) {
				std::cout << "Element with KEY=" << key << " exists!" << std::endl;
				std::string newPayload = incrementPayload((char*)element->Payload);

				if (OS15_LIB::OS15_HTCOM::Update_HT(h, handle, element, newPayload.c_str(), newPayload.length() + 1)) {
					std::cout << "Element updated! KEY=" << key << "; NEW PAYLOAD=" << newPayload << std::endl;
				}
				else {
					std::cerr << "Update failed" << std::endl;
				}
			}
			else {
				std::cerr << "Get failed. Element does not exist yet" << std::endl;
			}


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

int charToInt(char* str)
{
	std::stringstream convert;
	convert << str;
	int num;
	convert >> num;

	return num;
}

std::string incrementPayload(char* str)
{
	int oldNumberPayload = charToInt(str);
	int newNumberPayload = oldNumberPayload + 1;
	std::string newPayload = intToString(newNumberPayload);

	return newPayload;
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