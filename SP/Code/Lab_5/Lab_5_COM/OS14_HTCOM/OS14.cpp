#include "pch.h"
#include "OS14.h"

#ifdef _WIN64
#pragma comment(lib,"../../Lab_5_HT/x64/Debug/Lab_5_HT.lib")
#else
#pragma comment(lib,"../../Lab_5_HT/Debug/Lab_5_HT.lib")
#endif

HRESULT __stdcall OS14::QueryInterface(REFIID riid, void** ppv) {
	if (!ppv) {
		return E_POINTER;
	}
	if (riid == IID_IUnknown) {
		*ppv = (IHT*)this;
	}
	else if (riid == IID_IHT) {
		*ppv = (IHT*)this;
	}
	else {
		*ppv = nullptr;
		return E_NOINTERFACE;
	}

	((IUnknown*)*ppv)->AddRef();
	return S_OK;
}


ULONG __stdcall OS14::AddRef() {
	return InterlockedIncrement(&m_ref);
}

ULONG __stdcall OS14::Release() {
	if (InterlockedDecrement(&m_ref) == 0) {
		delete this;
		return 0;
	}
	return m_ref;
}

HRESULT __stdcall OS14::COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char UserGroup[512], const char FileName[512]) {
	*ppHandle = HT::Create(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, UserGroup, FileName);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Open(HT::HTHANDLE** ppHandle, const char FileName[512]) {
	*ppHandle = HT::Open(FileName);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Open(HT::HTHANDLE** ppHandle, const char User[256], const char Password[256], const char FileName[512]) {
	*ppHandle = HT::Open(User, Password, FileName);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Snap(BOOL& rc, HT::HTHANDLE* Handle) {
	rc = HT::Snap(Handle);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Close(BOOL& rc, HT::HTHANDLE* Handle) {
	rc = HT::Close(Handle);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element) {
	rc = HT::Insert(Handle, Element);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element) {
	rc = HT::Delete(Handle, Element);

	return S_OK;
}

HRESULT __stdcall OS14::COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* Element) {
	*ppElement = HT::Get(Handle, Element);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element, const void* NewPayload, int NewPayloadLength) {
	rc = HT::Update(Handle, Element, NewPayload, NewPayloadLength);
	return S_OK;
}

HRESULT __stdcall OS14::COM_Print(HT::Element* Element) {
	HT::Print(Element);
	return S_OK;
}

HRESULT __stdcall OS14::COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength) {
	*ppElement = new HT::Element(Key, KeyLength, Payload, PayloadLength);
	return S_OK;
}

HRESULT __stdcall OS14::COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength) {
	*ppElement = new HT::Element(Key, KeyLength);
	return S_OK;
}

HRESULT __stdcall OS14::COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	*ppElement = new HT::Element(OldElement, NewPayload, NewPayloadLength);
	return S_OK;
}