#pragma once
#include <Unknwn.h>
#include <iostream>


// {63397472-DFE9-438F-B9A8-FEC687B822B0}
static const GUID CLSID_OS13 =
{ 0x63397472, 0xdfe9, 0x438f, { 0xb9, 0xa8, 0xfe, 0xc6, 0x87, 0xb8, 0x22, 0xb0 } };



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
