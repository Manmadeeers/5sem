#pragma once
#include "../../Lab_5_HT/Lab_5_HT/Lab_5_HT.h"
#include <Unknwn.h>


// {DB767977-9B44-4C94-A33E-FE1FFEAA9E0E}
static const GUID IID_IHT =
{ 0xdb767977, 0x9b44, 0x4c94, { 0xa3, 0x3e, 0xfe, 0x1f, 0xfe, 0xaa, 0x9e, 0xe } };


__interface IHT :IUnknown {
	virtual HRESULT __stdcall COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char UserGroup[512], const char FileName[512])PURE;
	virtual HRESULT __stdcall COM_Open(HT::HTHANDLE** ppHandle, const char FileName[512])PURE;
	virtual HRESULT __stdcall COM_Open(HT::HTHANDLE** ppHandle, const char User[256], const char Password[256], const char FileName[512])PURE;
	virtual HRESULT __stdcall COM_Snap(BOOL& rc, HT::HTHANDLE* Handle)PURE;
	virtual HRESULT __stdcall COM_Close(BOOL& rc, HT::HTHANDLE* Handle)PURE;
	virtual HRESULT __stdcall COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element)PURE;
	virtual HRESULT __stdcall COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element)PURE;
	virtual HRESULT __stdcall COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* Element)PURE;
	virtual HRESULT __stdcall COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element, const void* NewPayload, int NewPayloadLength)PURE;
	virtual HRESULT __stdcall COM_Print(HT::Element* Element)PURE;
	virtual HRESULT __stdcall COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength)PURE;
	virtual HRESULT __stdcall COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength)PURE;
	virtual HRESULT __stdcall COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength)PURE;
};
