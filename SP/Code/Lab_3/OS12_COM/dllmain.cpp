#include "pch.h"
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <combaseapi.h>
#include "MathFactory.h"


HMODULE hModule;

// {36D75285-65E5-479E-B93C-CE8D25930BA0}
static const CLSID CLSID_CA =
{ 0x36d75285, 0x65e5, 0x479e, { 0xb9, 0x3c, 0xce, 0x8d, 0x25, 0x93, 0xb, 0xa0 } };


const WCHAR* FNAME = L"OS12_COMmnmd.dll";
const WCHAR* VerInd = L"OS12_COMmnmd.1.0";
const WCHAR* ProgId = L"OS12_COMmnmd.1";


BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    std::cout << "DllMain\n";
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hModule = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


HRESULT __declspec(dllexport) DllInstall(bool b, PCWSTR s)
{
    return S_OK;
}

HRESULT __declspec(dllexport) DllRegisterServer() {
    return RegisterServer(hModule, CLSID_CA, FNAME, VerInd, ProgId);
}

HRESULT __declspec(dllexport) DllUnregisterServer() {
    return UnregisterServer(CLSID_CA, VerInd, ProgId);
}


STDAPI DllCanUnloadNow()
{
    return S_OK;
}

STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, LPVOID* ppv) {
    HRESULT rc = E_UNEXPECTED;
    MathFactory* pF;
    if (clsid != CLSID_CA) rc = CLASS_E_CLASSNOTAVAILABLE;
    else if ((pF = new MathFactory()) == NULL) rc = E_OUTOFMEMORY;
    else {
        rc = pF->QueryInterface(iid, ppv);
        pF->Release();
    }
    return rc;
}

