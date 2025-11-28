#include "OS13_HTCOM.h"
#include "Connector.h"

class CHTStorage :public IHTStorage {
	std::atomic<ULONG>m_ref;
public:
	CHTStorage() :m_ref(1){
		InterlockedIncrement(&g_cObjects);
	}
	virtual ~CHTStorage() {
		InterlockedDecrement(&g_cObjects);
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override{
		if (!ppv) {
			return E_POINTER;
		}
		*ppv = nullptr;
		if (riid == IID_IUnknown || riid == IID_IHTStorage){
			*ppv = static_cast<IHTStorage*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHODIMP_(ULONG) AddRef() override{
		return ++m_ref;
	}
	STDMETHODIMP_(ULONG) Release() override{
		ULONG v = --m_ref;
		if (v == 0) {
			delete this;
		}
		return m_ref;
	}
	STDMETHODIMP CreateStorage(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512],/*out*/IHTStorage** ppHandle) override {
		if (!ppHandle) {
			return E_POINTER;
		}
		*ppHandle = nullptr;
		HTHANDLE*handle =
	}

};