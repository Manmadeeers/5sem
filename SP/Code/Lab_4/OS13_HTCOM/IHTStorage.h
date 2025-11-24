#pragma once
#include <Windows.h>
#include <Unknwn.h>
#include <iostream>
#include "../../Lab_2/OS11_HTAPI/OS_11DLL.h"

// {38BD7D03-CB43-4CB1-BDE5-3FF8A8AAA2DB}
static const GUID IID_IHTStorage =
{ 0x38bd7d03, 0xcb43, 0x4cb1, { 0xbd, 0xe5, 0x3f, 0xf8, 0xa8, 0xaa, 0xa2, 0xdb } };

__interface IHTStorage :public IUnknown {
	virtual HRESULT __stdcall Create(int capacity, int secSnapshotInterval, int maxKeyLength, int maxPayloadLength, const char fileName[512], HT::HTHANDLE& handle)PURE;
	virtual HRESULT __stdcall Open(const char filaName[512], HT::HTHANDLE& handle)PURE;
	virtual HRESULT __stdcall Snap(HT::HTHANDLE* handle, BOOL& rc)PURE;
	virtual HRESULT __stdcall Close(HT::HTHANDLE* handle, BOOL& rc)PURE;
	virtual HRESULT __stdcall Insert(HT::HTHANDLE* handle, HT::Element* element, BOOL& rc)PURE;
	virtual HRESULT __stdcall Delete(HT::HTHANDLE* handle, HT::Element* element, BOOL& rc)PURE;
	virtual HRESULT __stdcall Get(HT::HTHANDLE* handle, HT::Element* element, HT::Element& rc)PURE;
	virtual HRESULT __stdcall Update(HT::HTHANDLE* handle, HT::Element* oldElement, const void* newPayload, int newPayloadLength, BOOL& rc)PURE;
	virtual HRESULT __stdcall Print(HT::Element* element);
};

// {0B0CF976-6740-4B49-B055-9FB57B814748}
static const GUID IID_IElement =
{ 0xb0cf976, 0x6740, 0x4b49, { 0xb0, 0x55, 0x9f, 0xb5, 0x7b, 0x81, 0x47, 0x48 } };

__interface IElement :IUnknown {
	virtual HRESULT __stdcall ElementInsertConstructor(const void* key, int keyLength, const void* payload, int payloadLength,HT::Element&insertElement)PURE;
	virtual HRESULT __stdcall ElementGetConstructor(const void* key, int keyLength,HT::Element&getElement)PURE;
	virtual HRESULT __stdcall ElementUpdateConstructor(HT::Element* oldElement, const void* newPayload, int newPayloadLength, HT::Element& updateElement)PURE;
};
