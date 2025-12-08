#pragma once
#include "pch.h"
#include <Unknwn.h>
#include "../../Lab_1/lab1/OS10_HTAPI/pch.h"
#include "../../Lab_1/Lab1/OS10_HTAPI/OS_10_2.h"

// {6D6FC827-5121-4861-B971-58668542BA23}
static const GUID IID_IHT =
{ 0x6d6fc827, 0x5121, 0x4861, { 0xb9, 0x71, 0x58, 0x66, 0x85, 0x42, 0xba, 0x23 } };

// {A61D3D18-8A94-429E-8F67-FAD1C5FF6FD5}
static const GUID CLSID_OS13 =
{ 0xa61d3d18, 0x8a94, 0x429e, { 0x8f, 0x67, 0xfa, 0xd1, 0xc5, 0xff, 0x6f, 0xd5 } };


__interface IHT :public IUnknown {
	STDMETHOD(COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName))PURE;
	STDMETHOD(COM_Open(HT::HTHANDLE** ppHandle, const char* FileName))PURE;
	STDMETHOD(COM_Snap(BOOL& rc, HT::HTHANDLE* Handle))PURE;
	STDMETHOD(COM_CLose(BOOL& rc, HT::HTHANDLE* Handle))PURE;
	STDMETHOD(COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element))PURE;
	STDMETHOD(COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element))PURE;
	STDMETHOD(COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* Element))PURE;
	STDMETHOD(COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength))PURE;
	STDMETHOD(COM_Print(HT::Element* Element))PURE;
};

// {50A7CBD6-126A-4D0A-8A9B-4AD0B879DCB8}
static const GUID IID_IElement =
{ 0x50a7cbd6, 0x126a, 0x4d0a, { 0x8a, 0x9b, 0x4a, 0xd0, 0xb8, 0x79, 0xdc, 0xb8 } };


__interface IElement :public IUnknown {
	STDMETHOD(COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength))PURE;
	STDMETHOD(COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength))PURE;
	STDMETHOD(COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength))PURE;
};

