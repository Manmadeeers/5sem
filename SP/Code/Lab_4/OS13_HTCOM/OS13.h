#pragma once
#include "IHTStorage.h"

extern ULONG g_cComponents;
extern ULONG g_cServerLocks;

class OS13 :public IHTStorage, public IElement {
public:
	OS13() :m_ref(1) {
		InterlockedIncrement(&g_cComponents);
	}
	~OS13() {
		InterlockedDecrement(&g_cComponents);
		std::cout << "OS13 COM component self distruction called..." << std::endl;
	}
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

private:
	ULONG volatile m_ref;
};