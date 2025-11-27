#include "pch.h"
#include "StorageFactory.h"

StorageFactory::StorageFactory() :m_ref(1) {}
StorageFactory:: ~StorageFactory() {}

STDMETHODIMP StorageFactory::QueryInterface(REFIID riid, void** ppv){
	if (!ppv) {
		return E_POINTER;
	}
	*ppv = nullptr;

	if (riid == IID_IUnknown || riid == IID_IClassFactory) {
		*ppv = static_cast<IClassFactory*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) StorageFactory::AddRef(){
	return ++m_ref;
}

STDMETHODIMP_(ULONG) StorageFactory::Release(){
	ULONG v = --m_ref;
	if (v == 0) {
		delete this;
	}
	return v;
}

STDMETHODIMP StorageFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject){

	if (!ppvObject) {
		return E_POINTER;
	}

	*ppvObject = nullptr;

	if (pUnkOuter) {
		return CLASS_E_NOAGGREGATION;
	}

	CHTStorage* store = new (std::nothrow) CHTStorage();

	if (!store) {
		return E_OUTOFMEMORY;
	}

	HRESULT hr = store->QueryInterface(riid, ppvObject);
	store->Release();

	return hr;

}

STDMETHODIMP StorageFactory::LockServer(BOOL flock){
	if (flock) {
		InterlockedIncrement(&g_serverLocks);
	}
	else {
		InterlockedDecrement(&g_serverLocks);
	}
	return S_OK;
}

