#include "pch.h"

HMODULE g_hModule;                                          
const wchar_t* g_szFriendlyName = L"OS13.ComponentHT.COM";   
const wchar_t* g_szVerIndProgID = L"OS13.ComponentHT.1";     
const wchar_t* g_szProgID = L"OS13.ComponentHT";             
long g_cComponents = 0;		                                
long g_cServerLocks = 0;                                  


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



extern "C" __declspec(dllexport) STDAPI DllCanUnloadNow()
{
	if ((g_cComponents == 0) && (g_cServerLocks == 0))
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}


extern "C" __declspec(dllexport) STDAPI DllGetClassObject(const CLSID& clsid,
	const IID& iid,
	void** ppv)
{
	std::cout << "DllGetClassObject: Create ClassFactory" << std::endl;

	if (clsid != CLSID_OS13)
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	HTFactory* pFactory = new HTFactory;
	if (pFactory == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = pFactory->QueryInterface(iid, ppv);
	pFactory->Release();

	return hr;
}

extern "C" __declspec(dllexport) HRESULT __stdcall DllRegisterServer()
{
	return RegisterServer(g_hModule,
		CLSID_OS13,
		g_szFriendlyName,
		g_szVerIndProgID,
		g_szProgID);
}

extern "C" __declspec(dllexport) HRESULT __stdcall DllUnregisterServer()
{
	return UnregisterServer(CLSID_OS13,
		g_szVerIndProgID,
		g_szProgID);
}

