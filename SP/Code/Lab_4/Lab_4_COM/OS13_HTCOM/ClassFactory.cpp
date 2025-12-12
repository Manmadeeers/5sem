#include "pch.h"
#include "ClassFactory.h"
#include "OS13.h"

HRESULT __stdcall OS13Factory::QueryInterface(REFIID riid, void** ppv) {
	if (riid == IID_IUnknown || riid == IID_IClassFactory) {
		*ppv = (IClassFactory*)this;
	}
	else {
		*ppv = nullptr;
		return E_NOINTERFACE;
	}

	((IUnknown*)*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall OS13Factory::AddRef() {
	return InterlockedIncrement(&m_Cref);
}
ULONG __stdcall OS13Factory::Release() {
	if (InterlockedDecrement(&m_Cref)) {
		delete this;
		return 0;
	}
	return m_Cref;
}

HRESULT __stdcall OS13Factory::CreateInstance(IUnknown* pUnknownOuter, REFIID iid, void** ppv) {
	if (pUnknownOuter != NULL)
	{
		return CLASS_E_NOAGGREGATION;
	}
	OS13* pOs = new OS13();
	if (pOs == NULL)
	{
		return E_OUTOFMEMORY;
	}
}

