#include "pch.h"
#include "Factory.h"
#include "OS15.h"

STDMETHODIMP Factory::QueryInterface(REFIID riid, void** ppv) {
	if (!ppv) {
		return E_POINTER;
	}
	*ppv = nullptr;
	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
		*ppv = static_cast<IClassFactory*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) Factory::AddRef() {
	return InterlockedIncrement(&m_ref);
}

STDMETHODIMP_(ULONG) Factory::Release() {
	ULONG res = InterlockedDecrement(&m_ref);
	if (res == 0) {
		delete this;
	}

	return res;
}

STDMETHODIMP Factory::CreateInstance(IUnknown* pUnknownOuter, REFIID iid, void** ppv) {
	if (pUnknownOuter != nullptr) {
		return CLASS_E_NOAGGREGATION;
	}
	if (!ppv) {
		return E_POINTER;
	}
	OS15* pOS15 = new OS15;
	if (pOS15 == nullptr) {
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pOS15->QueryInterface(iid, ppv);
	pOS15->Release();
	return hr;
}

STDMETHODIMP Factory::LockServer(BOOL fLock) {
	if (fLock) {
		InterlockedIncrement(&g_cServerLocks);
	}
	else {
		InterlockedDecrement(&g_cServerLocks);
	}

	return S_OK;
}