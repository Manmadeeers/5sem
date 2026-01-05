#include "pch.h"
#include "Factory.h"
#include "OS14.h"

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
	OS14* pOS14 = new OS14;
	if (pOS14 == nullptr) {
		return E_OUTOFMEMORY;
	}
	HRESULT hr = pOS14->QueryInterface(iid, ppv);
	pOS14->Release();
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