#pragma once
#include <Unknwn.h>
extern long g_cComponents;
extern long g_cServerLocks;

class OS13Factory :public IClassFactory {
public:
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv);
	virtual HRESULT __stdcall LockServer(BOOL fLock);

	OS13Factory() :m_ref(1) {}
	~OS13Factory(){}
private:
	volatile long m_Cref;
};