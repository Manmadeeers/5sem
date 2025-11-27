#include "pch.h"
#include "StorageFactory.h"



class StorageFactory :public IClassFactory {
	std::atomic<ULONG>m_ref;
public:
	StorageFactory() :m_ref(1) {}
	virtual ~StorageFactory() {}
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
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
	STDMETHODIMP_(ULONG) AddRef() {
		return ++m_ref;
	}
	STDMETHODIMP_(ULONG)Release() {
		ULONG v = --m_ref;
		if (v == 0) {
			delete this;
		}
		return v;
	}
	STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) {

		if (!ppvObject) {
			return E_POINTER;
		}

		*ppvObject = nullptr;

		if (pUnkOuter) {
			return CLASS_E_NOAGGREGATION;
		}

		CreateHTStorageInstance(riid, ppvObject);
	}

	STDMETHODIMP LockServer(BOOL flock) {
		if (flock) {
			InterlockedIncrement(&g_serverLocks);
		}
		else {
			InterlockedDecrement(&g_serverLocks);
		}
		return S_OK;
	}


};


extern "C" {
	__declspec(dllexport) HRESULT CreateStorageFactoryInstance(REFIID riid, void**ppv) {
		StorageFactory* sf = new (std::nothrow) StorageFactory();

		if (!sf) {
			return E_OUTOFMEMORY;
		}
		HRESULT hr = sf->QueryInterface(riid, ppv);
		sf->Release();
		return hr;
	}
}