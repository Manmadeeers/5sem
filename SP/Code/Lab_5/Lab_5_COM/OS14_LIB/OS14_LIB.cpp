#include "pch.h"
#include "OS14_LIB.h"

// {382B7449-EA6C-49B3-BB5A-31FE75A795B9}
static const GUID CLSID_OS14 =
{ 0x382b7449, 0xea6c, 0x49b3, { 0xbb, 0x5a, 0x31, 0xfe, 0x75, 0xa7, 0x95, 0xb9 } };

IHT* pIHT = nullptr;


OS14_HANDLE OS14_LIB::Init() {
	IUnknown* pIUnknown = nullptr;

	CoInitialize(NULL);
	HRESULT hr0 = CoCreateInstance(CLSID_OS14, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if (SUCCEEDED(hr0)) {
		return pIUnknown;
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}


void OS14_LIB::Dispose(OS14_HANDLE h) {
	((IUnknown*)h)->Release();
	CoFreeUnusedLibraries();
	CoUninitialize();
}

HT::HTHANDLE* OS14_LIB::OS14_HTCOM::Create_HT(OS14_HANDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char UserGroup[512], const char FileName[512]) {
	HT::HTHANDLE** ppHT = new HT::HTHANDLE*;
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Create(ppHT, Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, UserGroup, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppHT;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::HTHANDLE* OS14_LIB::OS14_HTCOM::Open_HT(OS14_HANDLE h, const char FileName[512]) {
	HT::HTHANDLE** ppHT = new HT::HTHANDLE*;
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Open(ppHT, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppHT;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::HTHANDLE* OS14_LIB::OS14_HTCOM::Open_HT(OS14_HANDLE h, const char User[256], const char Password[256], const char FileName[512]) {
	HT::HTHANDLE** ppHT = new HT::HTHANDLE*;
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Open(ppHT, User, Password, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppHT;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

BOOL OS14_LIB::OS14_HTCOM::Snap_HT(OS14_HANDLE h, HT::HTHANDLE* Handle) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Snap(rc, Handle);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return rc;
		}
		else {
			pIHT->Release();
			return rc;
		}
	}
	else {
		throw (int)hr0;
		return rc;
	}
}

BOOL OS14_LIB::OS14_HTCOM::Close_HT(OS14_HANDLE h, HT::HTHANDLE* Handle) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Close(rc, Handle);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return rc;
		}
		else {
			pIHT->Release();
			return rc;
		}
	}
	else {
		throw (int)hr0;
		return rc;
	}
}

BOOL OS14_LIB::OS14_HTCOM::Insert_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Insert(rc, Handle, Element);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return rc;
		}
		else {
			pIHT->Release();
			return rc;
		}
	}
	else {
		throw (int)hr0;
		return rc;
	}
}

BOOL OS14_LIB::OS14_HTCOM::Delete_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Delete(rc, Handle, Element);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return rc;
		}
		else {
			pIHT->Release();
			return rc;
		}
	}
	else {
		throw (int)hr0;
		return rc;
	}
}

HT::Element* OS14_LIB::OS14_HTCOM::Get_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	HT::Element** pElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Get(pElement, Handle, Element);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *pElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

BOOL OS14_LIB::OS14_HTCOM::Update_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Update(rc, Handle, OldElement, NewPayload, NewPayloadLength);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return rc;
		}
		else {
			pIHT->Release();
			return rc;
		}
	}
	else {
		throw (int)hr0;
		return rc;
	}
}

void OS14_LIB::OS14_HTCOM::Print_HT(OS14_HANDLE h, HT::Element* Element) {
	BOOL rc = FALSE;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Print(Element);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
		}
		else {
			pIHT->Release();
		}
	}
	else {
		throw (int)hr0;
	}
}

HT::Element* OS14_LIB::OS14_HTCOM::ConstructInsertElement_HT(OS14_HANDLE h, const void* Key, int KeyLength, const void* Payload, int PayloadLength) {
	HT::Element** ppElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_ConstructInsertElement(ppElement, Key, KeyLength, Payload, PayloadLength);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::Element* OS14_LIB::OS14_HTCOM::ConstructGetElement_HT(OS14_HANDLE h, const void* Key, int KeyLength) {
	HT::Element** ppElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_ConstructGetElement(ppElement, Key, KeyLength);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::Element* OS14_LIB::OS14_HTCOM::ConstructUpdateElement_HT(OS14_HANDLE h, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	HT::Element** ppElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_ConstructUpdateElement(ppElement, OldElement, NewPayload, NewPayloadLength);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *ppElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

