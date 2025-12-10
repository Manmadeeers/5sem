#include "pch.h"
#include "Component.h"

#ifdef _WIN64
#pragma comment(lib,"../../Lab1/x64/Debug/OS10_HTAPI.lib")
#else
#pragma comment(lib,"../../Lab_1/x64/Debug/OS10_HTAPI.lib")
#endif

#pragma region Implementing IUnknown methods
HRESULT __stdcall OS13::QueryInterface(REFIID riid, void** ppv) {
	if (!ppv) {
		return E_POINTER;
	}

	if (IsEqualIID(riid, IID_IUnknown)) {
		*ppv = (IHT*)this;
	}
	else if (IsEqualIID(riid, IID_IElement)) {
		*ppv = (IElement*)this;
	}
	else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	((IUnknown*)*ppv)->AddRef();
	return S_OK;
}

ULONG __stdcall OS13::AddRef() {
	return InterlockedIncrement(&m_ref);
}
ULONG __stdcall OS13::Release() {
	if (InterlockedDecrement(&m_ref) == 0) {
		delete this;
		return 0;
	}
	return m_ref;
}

#pragma endregion

#pragma region Implemening IHT methods

STDMETHODIMP OS13::COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName) {
	if (!*ppHandle) {
		return E_INVALIDARG;
	}
	*ppHandle = HT::Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
	if (!*ppHandle) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Open(HT::HTHANDLE** ppHandle, const char* FileName) {
	if (!*ppHandle) {
		return E_INVALIDARG;
	}
	*ppHandle = HT::Open(FileName);
	if (!*ppHandle) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Snap(BOOL& rc, HT::HTHANDLE* Handle) {
	if (!rc) {
		return E_INVALIDARG;
	}
	rc = HT::Snap(Handle);
	if (!rc) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_CLose(BOOL& rc, HT::HTHANDLE* Handle) {
	if (!rc) {
		return E_INVALIDARG;
	}
	rc = HT::Close(Handle);
	if (!rc) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element) {
	if (!rc) {
		return E_INVALIDARG;
	}
	rc = HT::Insert(Handle, Element);
	if (!rc) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element) {
	if (!rc) {
		return E_INVALIDARG;
	}
	rc = HT::Delete(Handle, Element);
	if (!rc) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* Element) {
	if (!*ppElement) {
		return E_INVALIDARG;
	}
	*ppElement = HT::Get(Handle, Element);
	if (!*ppElement) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	if (!rc) {
		return E_INVALIDARG;
	}
	rc = HT::Update(Handle, OldElement, NewPayload, NewPayloadLength);
	if (!rc) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_Print(HT::Element* Element) {
	if (!Element) {
		return E_INVALIDARG;
	}
	HT::Print(Element);
	return S_OK;
}

#pragma endregion


#pragma region Implementing IElement methods

STDMETHODIMP OS13::COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength) {
	if (!*ppElement) {
		return E_INVALIDARG;
	}
	*ppElement = new HT::Element(Key, KeyLength);
	if (!*ppElement) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength) {
	if (!ppElement) {
		return E_INVALIDARG;
	}
	*ppElement = new HT::Element(Key, KeyLength, Payload, PayloadLength);
	if (!*ppElement) {
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP OS13::COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	if (!*ppElement) {
		return E_INVALIDARG;
	}
	*ppElement = new HT::Element(OldElement, NewPayload, NewPayloadLength);
	if (!*ppElement) {
		return E_FAIL;
	}	
	return S_OK;
}

#pragma endregion