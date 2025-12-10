#pragma once
#include "pch.h"
extern long g_cComponents;
extern long g_cServerLocks;


class HTFactory :public IClassFactory {
public:
	//IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	//IClassFactory
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter, REFIID riid, void** ppv);
	virtual HRESULT __stdcall LockServer(BOOL bLock);

	HTFactory() :m_ref(1){}
	~HTFactory(){
		std::cout << "HTFactory: Self destruction" << std::endl;
	}
private:
	long m_ref;
};