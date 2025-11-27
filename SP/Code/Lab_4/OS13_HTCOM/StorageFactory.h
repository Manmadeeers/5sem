#pragma once
#include "Connector.h"


class StorageFactory :public IClassFactory {
	std::atomic<ULONG>m_ref;
public:
	StorageFactory();
	virtual ~StorageFactory();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG)Release();
	STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject);
	STDMETHODIMP LockServer(BOOL flock);
};