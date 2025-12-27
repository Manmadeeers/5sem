#include "pch.h"
#include "OS13_LIB.h"

// {63397472-DFE9-438F-B9A8-FEC687B822B0}
static const GUID CLSID_OS13 =
{ 0x63397472, 0xdfe9, 0x438f, { 0xb9, 0xa8, 0xfe, 0xc6, 0x87, 0xb8, 0x22, 0xb0 } };

IHT* pIHT = nullptr;


OS13_HANDLE OS13_LIB::Init() {
	IUnknown* pIUnknown = nullptr;

	CoInitialize(NULL);
	HRESULT hr0 = CoCreateInstance(CLSID_OS13, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);
	if (SUCCEEDED(hr0)) {
		return pIUnknown;
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

void OS13_LIB::Dispose(OS13_HANDLE h) {
	((IUnknown*)h)->Release();
	CoFreeUnusedLibraries();
	CoUninitialize();
}

HT::HTHANDLE* OS13_LIB::OS13_HTCOM::Create_HT(OS13_HANDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {
	HT::HTHANDLE** pHT = new HT::HTHANDLE*;
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Create(pHT, Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *pHT;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}

}


HT::HTHANDLE* OS13_LIB::OS13_HTCOM::Open_HT(OS13_HANDLE h, const char FileName[512]) {
	HT::HTHANDLE** pHT = new HT::HTHANDLE*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Open(pHT, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *pHT;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

BOOL OS13_LIB::OS13_HTCOM::Snap_HT(OS13_HANDLE h, HT::HTHANDLE* Handle) {
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


BOOL OS13_LIB::OS13_HTCOM::Close_HT(OS13_HANDLE h, HT::HTHANDLE* Handle) {
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


BOOL OS13_LIB::OS13_HTCOM::Insert_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
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

BOOL OS13_LIB::OS13_HTCOM::Update_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
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

BOOL OS13_LIB::OS13_HTCOM::Delete_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
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

HT::Element* OS13_LIB::OS13_HTCOM::Get_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
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

void OS13_LIB::OS13_HTCOM::Print_HT(OS13_HANDLE h, HT::Element* Element) {
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

HT::Element* OS13_LIB::OS13_HTCOM::ConstructInsertElement_HT(OS13_HANDLE h, const void* Key, int KeyLength, const void* Payload, int PayloadLength) {
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

HT::Element* OS13_LIB::OS13_HTCOM::ConstructGetElement_HT(OS13_HANDLE h, const void* Key, int KeyLength) {
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

HT::Element* OS13_LIB::OS13_HTCOM::ConstructUpdateElement_HT(OS13_HANDLE h, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
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


