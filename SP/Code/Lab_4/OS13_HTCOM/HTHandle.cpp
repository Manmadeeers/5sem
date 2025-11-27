#include "pch.h"
#include "HTHandle.h"


class CHTHandle :public IHTHandle {
	std::atomic<ULONG>m_ref;
	HT::HTHANDLE* m_nativeHandle;
public:
	CHTHandle(HT::HTHANDLE* nativeHandle) :m_ref(1), m_nativeHandle(nativeHandle) {
		InterlockedIncrement(&g_serverLocks);
	}
	virtual ~CHTHandle() {
		if (m_nativeHandle) {
			HT::Close(m_nativeHandle);
			m_nativeHandle = nullptr;
		}

		InterlockedDecrement(&g_serverLocks);
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
		if (!ppv) {
			return E_POINTER;
		}
		*ppv = nullptr;

		if (riid == IID_IUnknown || riid == IID_IHTHandle) {
			*ppv = static_cast<IHTHandle*>(this);
			AddRef();
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef() override {
		return ++m_ref;
	}
	STDMETHODIMP_(ULONG)Release() override {
		ULONG v = --m_ref;
		if (v == 0) {
			delete this;
		}
		return v;
	}


	STDMETHODIMP Insert(const ElementStruct* element,/*out*/ BOOL* pbSuccess) override {
		if (!element || !pbSuccess) {
			return E_POINTER;
		}
		HT::Element nativeElement;
		nativeElement.key = element->Key.empty() ? nullptr : element->Key.data();
		nativeElement.keylength = (int)element->KeyLength;
		nativeElement.payload = element->Payload.empty() ? nullptr : element->Payload.data();
		nativeElement.payloadlength = (int)element->PayloadLength;

		BOOL res = HT::Insert(m_nativeHandle, &nativeElement);
		*pbSuccess = res ? TRUE : FALSE;
		return S_OK;
	}

	STDMETHODIMP Delete(const ElementStruct* element, /*out*/BOOL* pbSuccess)override {
		if (!element || !pbSuccess) {
			return E_POINTER;
		}
		HT::Element nativeElement;
		nativeElement.key = element->Key.empty() ? nullptr : element->Key.data();
		nativeElement.keylength = (int)element->KeyLength;
		nativeElement.payload = element->Payload.empty() ? nullptr : element->Payload.data();
		nativeElement.payloadlength = (int)element->PayloadLength;

		BOOL res = HT::Delete(m_nativeHandle, &nativeElement);
		*pbSuccess = res ? TRUE : FALSE;
		return S_OK;
	}

	STDMETHODIMP Get(const ElementStruct* searchElement,/*out*/ ElementStructReturn** ppFoundElement)override {
		if (!searchElement || !ppFoundElement) {
			return E_POINTER;
		}

		*ppFoundElement = nullptr;
		HT::Element search;

		search.key = searchElement->Key.empty() ? nullptr : searchElement->Key.data();
		search.keylength = (int)searchElement->KeyLength;
		search.payload = nullptr;
		search.payloadlength = 0;

		HT::Element* found = HT::Get(m_nativeHandle, &search);
		if (!found) {
			return S_FALSE;
		}

		ElementStructReturn* out = (ElementStructReturn*)CoTaskMemAlloc(sizeof(ElementStructReturn));
		if (!out) {
			delete found;
			return E_OUTOFMEMORY;
		}

		ZeroMemory(out, sizeof(*out));

		if (found->key && found->keylength > 0) {
			out->KeyLength = (uint32)found->keylength;
			out->Key = (BYTE*)CoTaskMemAlloc(out->KeyLength);
			if (!out->Key) {
				FreeReturnedElement(out);
				delete found;
				return E_OUTOFMEMORY;
			}
			memcpy(out->Key, found->key, out->KeyLength);
		}
		else {
			out->Key = nullptr;
			out->KeyLength = 0;
		}

		if (found->payload && found->payloadlength > 0) {
			out->PayloadLength = (uint32)found->payloadlength;
			out->Payload = (BYTE*)CoTaskMemAlloc(out->PayloadLength);
			if (!out->Payload) {
				FreeReturnedElement(out);
				delete found;
				return E_OUTOFMEMORY;
			}
			memcpy(out->Payload, found->payload, out->PayloadLength);
		}
		else {
			out->Payload = nullptr;
			out->PayloadLength = 0;
		}

		delete found;
		*ppFoundElement = out;
		return S_OK;
	}

	STDMETHODIMP Update(const ElementStruct* oldElement, const BYTE* newPayload, uint32 newPayloadLength, /*out*/BOOL* pbSuccess)override {
		if (!oldElement || !pbSuccess) {
			return E_POINTER;
		}

		HT::Element nativeOld;
		nativeOld.key = oldElement->Key.empty() ? nullptr : oldElement->Key.data();
		nativeOld.keylength = (int)oldElement->KeyLength;
		nativeOld.payload = nullptr;
		nativeOld.payloadlength = 0;

		BOOL res = HT::Update(m_nativeHandle, &nativeOld, (void*)newPayload, (int)newPayloadLength);
		*pbSuccess = res ? TRUE : FALSE;
		return S_OK;
	}

	STDMETHODIMP Snap(BOOL* pbSuccess)override {
		if (!pbSuccess) {
			return E_POINTER;
		}

		BOOL res = HT::Snap(m_nativeHandle);
		*pbSuccess = res ? TRUE : FALSE;
		return S_OK;
	}

	STDMETHODIMP Close(/*out*/BOOL* pbSuccess)override {
		if (!pbSuccess) {
			return E_POINTER;
		}

		BOOL res = HT::Close(m_nativeHandle);
		if (res) {
			m_nativeHandle = nullptr;
			*pbSuccess = TRUE;
			return S_OK;
		}
		else {
			*pbSuccess = FALSE;
			return E_FAIL;
		}
	}


	STDMETHODIMP Print(const ElementStruct* element) override {
		if (!element) {
			return E_POINTER;
		}
		HT::Element nativeEl;
		nativeEl.key = element->Key.empty() ? nullptr : element->Key.data();
		nativeEl.keylength = (int)element->KeyLength;
		nativeEl.payload = element->Payload.empty() ? nullptr : element->Payload.data();
		nativeEl.payloadlength = (int)element->PayloadLength;

		HT::Print(&nativeEl);
		return S_OK;
	}

	STDMETHODIMP CreateSnapshotFileName(BSTR* pSnapshotFileName) override {
		if (!pSnapshotFileName) {
			return E_POINTER;
		}
		const char* cstr = HT::CreateSnapshotFileName(m_nativeHandle);
		if (!cstr) {
			*pSnapshotFileName = nullptr;
			return S_FALSE;
		}

		int required = MultiByteToWideChar(CP_UTF8, 0, cstr, -1, nullptr, 0);
		if (required == 0) {
			return E_FAIL;
		}

		BSTR b = SysAllocStringLen(NULL, required - 1);
		MultiByteToWideChar(CP_UTF8, 0, cstr, -1, b, required);
		*pSnapshotFileName = b;
		return S_OK;
	}
	STDMETHODIMP GetHandleInfo(HTHandleInfo* pInfo)override {
		if (!pInfo) {
			return E_POINTER;
		}
		if (!m_nativeHandle) {
			return E_FAIL;
		}

		pInfo->Capacity = (uint32)m_nativeHandle->Capacity;
		pInfo->SecSnapshotInterval = (uint32)m_nativeHandle->SecSnapshotInterval;
		pInfo->MaxKeyLength = (uint32)m_nativeHandle->MaxKeyLength;
		pInfo->MaxPayloadLength = (uint32)m_nativeHandle->MaxPayloadLength;
		pInfo->CurrentElements = (uint32)m_nativeHandle->CurrentElements;

		size_t out = 0;
		mbstowcs_s(&out, pInfo->FileName, m_nativeHandle->FileName, _TRUNCATE);
		mbstowcs_s(&out, pInfo->CrossProcessMutexName, m_nativeHandle->mutex_name, _TRUNCATE);

		return S_OK;

	}
};


extern "C" {
	__declspec(dllexport) HRESULT CreateHTHandleInstance(HT::HTHANDLE* nativeHandle, IUnknown** ppHandle) {
		if (!ppHandle) {
			return E_POINTER;
		}
		*ppHandle = nullptr;

		try {

			CHTHandle* obj = new CHTHandle(nativeHandle);
			*ppHandle = static_cast<IHTHandle*>(obj);

			return S_OK;
		}
		catch (std::bad_alloc&) {
			HT::Close(nativeHandle);
			return E_OUTOFMEMORY;
		}
		catch (...) {
			return E_FAIL;
		}
	}
}