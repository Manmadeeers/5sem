#pragma once
#include <Unknwn.h>
#include <iostream>

// {382B7449-EA6C-49B3-BB5A-31FE75A795B9}
static const GUID CLSID_OS14 =
{ 0x382b7449, 0xea6c, 0x49b3, { 0xbb, 0x5a, 0x31, 0xfe, 0x75, 0xa7, 0x95, 0xb9 } };


class Factory :public IClassFactory {
public:
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter, REFIID iid, void** ppv);
	virtual HRESULT __stdcall LockServer(BOOL fLock);


	Factory() :m_ref(1) {}
	~Factory() {
		std::cout << "Factory: Destroying class factory" << std::endl;
	}
private:
	volatile long m_ref;
};