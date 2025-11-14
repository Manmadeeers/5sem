#pragma once
#include <assert.h>
#include <objbase.h>

void CLSIDtochar(
	const CLSID& clsid,      // Convert a CLSID to a char string.   
	WCHAR* szCLSID,          // smw:char* szCLSID,
	int length);

LONG recursiveDeleteKey(
	HKEY hKeyParent,           // Parent of key to delete
	const WCHAR* lpszKeyChild);

BOOL setKeyAndValue(
	const WCHAR* szKey,         // smw:const char* szKey
	const WCHAR* szSubkey,      // smw:const char* szSubkey,
	const WCHAR* szValue);

HRESULT RegisterServer(
	HMODULE hModule,            // DLL module handle
	const CLSID& clsid,         // Class ID
	const WCHAR* szFriendlyName, // Friendly Name
	const WCHAR* szVerIndProgID, // Programmatic
	const WCHAR* szProgID);

HRESULT UnregisterServer(
	const CLSID& clsid,
	const WCHAR* szVerIndProgID,
	const WCHAR* szProgID);