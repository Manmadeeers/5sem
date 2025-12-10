#include "pch.h"

#pragma region Implementing IUnknown methods

HRESULT __stdcall HTFactory::QueryInterface(REFIID riid, void** ppv) {
	if (IsEqualIID(riid, IID_IUnknown)||IsEqualIID(riid,IID_IClassFactory)) {
		*ppv = (IClassFactory*)this;
	}
	else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	((IUnknown*)*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall HTFactory::AddRef() {
	return InterlockedIncrement(&m_ref);
}

ULONG __stdcall HTFactory::Release() {
	if (InterlockedDecrement(&m_ref)==0) {
		delete this;
		return 0;
	}
	return m_ref;
}

#pragma endregion


#pragma region Implementing IClassFacotry methods

HRESULT __stdcall HTFactory::CreateInstance(IUnknown* pUnknownOuter, REFIID riid, void** ppv) {
	std::cout << "HTFactory: Create a component" << std::endl;

	if (pUnknownOuter!=NULL) {
		return CLASS_E_NOAGGREGATION;
	}

	OS13* pOS = new OS13;

	if (pOS == NULL) {
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pOS->QueryInterface(riid, ppv);
	pOS->Release();
	return hr;
}

HRESULT __stdcall HTFactory::LockServer(BOOL bLock) {
	if (bLock) {
		InterlockedIncrement(&g_cServerLocks);
	}
	else {
		InterlockedDecrement(&g_cServerLocks);
	}
	return S_OK;
}

#pragma endregion
