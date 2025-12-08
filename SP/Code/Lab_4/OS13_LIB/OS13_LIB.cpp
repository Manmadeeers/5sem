#include "pch.h"
#include "OS13_LIB.h"

IHT* pIHT = nullptr;
IElement* pIElement = nullptr;

#pragma region Implementing Init and Dispose

OS13_HTCOM_HAHNDLE OS13_LIB::Init() {
	IUnknown* pIUnknown = NULL;
	CoInitialize(NULL);

	HRESULT hr0 = CoCreateInstance(CLSID_OS13, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);

	if (SUCCEEDED(hr0)) {
		return pIUnknown;
	}
	else {
		throw (int)hr0;
		return NULL;
	}
}

void OS13_LIB::Dispose(OS13_HTCOM_HAHNDLE h) {
	((IUnknown*)h)->Release();
	CoFreeUnusedLibraries();
	CoUninitialize();
}

#pragma endregion

#pragma region Implementing HT_LIB namespace methods


HT::HTHANDLE* OS13_LIB::HT_LIB::LIB_Create(OS13_HTCOM_HAHNDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName) {
	HT::HTHANDLE** handle = new HT::HTHANDLE*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);

	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Create(handle, Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *handle;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::HTHANDLE* OS13_LIB::HT_LIB::LIB_Open(OS13_HTCOM_HAHNDLE h, const char* FileName) {

	HT::HTHANDLE** handle = new HT::HTHANDLE*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);

	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Open(handle, FileName);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *handle;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

BOOL OS13_LIB::HT_LIB::LIB_Snap(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* handle) {
	BOOL rc = false;
	
	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Snap(rc, handle);
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

BOOL OS13_LIB::HT_LIB::LIB_Close(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* handle) {
	BOOL rc = false;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_CLose(rc, handle);
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


BOOL OS13_LIB::HT_LIB::LIB_Insert(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	BOOL rc = false;

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

BOOL OS13_LIB::HT_LIB::LIB_Delete(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	BOOL rc = false;

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


HT::Element* OS13_LIB::HT_LIB::LIB_Get(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element) {
	HT::Element** rcElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IHT, (void**)&pIHT);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIHT->COM_Get(rcElement, Handle, Element);
		if (!SUCCEEDED(hr1)) {
			pIHT->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIHT->Release();
			return *rcElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

BOOL OS13_LIB::HT_LIB::LIB_Update(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	BOOL rc = false;

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

void OS13_LIB::HT_LIB::LIB_Print(OS13_HTCOM_HAHNDLE h, HT::Element* Element) {
	BOOL rc = false;

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

#pragma endregion

HT::Element* OS13_LIB::ELEMENT_LIB::LIB_ConstructGetElement(OS13_HTCOM_HAHNDLE h, const void* Key, int KeyLength) {
	HT::Element** rcElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IElement, (void**)&pIElement);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIElement->COM_ConstructGetElement(rcElement, Key, KeyLength);
		if (!SUCCEEDED(hr1)) {
			pIElement->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIElement->Release();
			return *rcElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}


HT::Element* OS13_LIB::ELEMENT_LIB::LIB_ConstructInsertElement(OS13_HTCOM_HAHNDLE h, const void* Key, int KeyLength, const void* Payload, int PayloadLength) {
	HT::Element** rcElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IElement, (void**)&pIElement);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIElement->COM_ConstructInsertElement(rcElement, Key, KeyLength, Payload, PayloadLength);
		if (!SUCCEEDED(hr1)) {
			pIElement->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIElement->Release();
			return *rcElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}

HT::Element* OS13_LIB::ELEMENT_LIB::LIB_ConstructUpdateElement(OS13_HTCOM_HAHNDLE h, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength) {
	HT::Element** rcElement = new HT::Element*;

	HRESULT hr0 = ((IUnknown*)h)->QueryInterface(IID_IElement, (void**)&pIElement);
	if (SUCCEEDED(hr0)) {
		HRESULT hr1 = pIElement->COM_ConstructUpdateElement(rcElement, OldElement, NewPayload, NewPayloadLength);
		if (!SUCCEEDED(hr1)) {
			pIElement->Release();
			throw (int)hr1;
			return nullptr;
		}
		else {
			pIElement->Release();
			return *rcElement;
		}
	}
	else {
		throw (int)hr0;
		return nullptr;
	}
}