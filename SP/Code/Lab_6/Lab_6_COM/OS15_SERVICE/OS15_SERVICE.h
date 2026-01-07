#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <string>
#include "sddl.h"

#include "../OS15_LIB/OS15_LIB.h"

#define SERVICENAME L"OS15_SERVICE"
#define HTPATH "C:/ProgramData/delete/HT.ht"
#define WIDEPATH L"C:/ProgramData/delete/HT.ht"
#define USERNAME "HTUser01"
#define PASSWORD "1111"
#define TRACEPATH "C:/ProgramData/delete/service.trace"

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);
SECURITY_ATTRIBUTES getSecurityAttributes();
HANDLE createStopEvent(const wchar_t* stopEventName);
void startService();
void trace(const char* msg, int r = std::ofstream::app);
