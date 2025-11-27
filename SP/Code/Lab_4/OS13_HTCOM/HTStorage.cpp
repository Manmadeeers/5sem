#include "pch.h"
#include "HTStorage.h"



class CHTStorage : public IHTStorage {
	std::atomic<ULONG>m_ref;
public:
	CHTStorage() :m_ref(1) {
		InterlockedIncrement(&g_serverLocks);
	}

	virtual ~CHTStorage() {
		InterlockedDecrement(&g_serverLocks);
	}
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) {
		if (!ppv) {
			return E_POINTER;
		}
		*ppv = nullptr;
		if (riid == IID_IUnknown || riid == IID_IHTStorage) {
			*ppv = static_cast<IHTStorage*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHODIMP_(ULONG) AddRef() {
		return ++m_ref;
	}

	STDMETHODIMP_(ULONG) Release() {
		ULONG v = --m_ref;
		if (v == 0) {
			delete this;
		}
		return v;
	}
	STDMETHODIMP CrStorage(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName, IUnknown** ppHandle) {
		if (!ppHandle) {
			return E_POINTER;
		}

		*ppHandle = nullptr;


		HT::HTHANDLE* native = HT::Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);

		if (!native) {
			return E_FAIL;
		}

		CreateHTHandleInstance(native, ppHandle);
	}
	STDMETHODIMP Open(const char* FileName, IUnknown** ppHandle) {
		if (!ppHandle) {
			return E_POINTER;
		}


		*ppHandle = nullptr;

		HT::HTHANDLE* native = HT::Open((const char*)FileName);

		if (!native) {
			return E_FAIL;
		}

		CreateHTHandleInstance(native, ppHandle);
	}

};

extern "C" {
	__declspec(dllexport) HRESULT CreateHTStorageInstance(REFIID riid, void**ppvObject) {

		CHTStorage* store = new (std::nothrow) CHTStorage();

		if (!store) {
			return E_OUTOFMEMORY;
		}

		HRESULT hr = store->QueryInterface(riid, ppvObject);
		store->Release();

		return hr;

	}
}