#pragma once
#include <Unknwn.h>
#include <iostream>

// {59F0C722-5EC5-476D-8C08-A144FE1DD3AE}
static const GUID CLSID_OS15 =
{ 0x59f0c722, 0x5ec5, 0x476d, { 0x8c, 0x8, 0xa1, 0x44, 0xfe, 0x1d, 0xd3, 0xae } };

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
