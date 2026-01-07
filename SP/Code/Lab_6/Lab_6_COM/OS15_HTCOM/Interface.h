#pragma once
#include "../../Lab_6_HT/Lab_6_HT/Lab_6_HT.h"
#include <Unknwn.h>


// {6F157264-797B-40A9-8D48-45C9699553F9}
static const GUID IID_IHT =
{ 0x6f157264, 0x797b, 0x40a9, { 0x8d, 0x48, 0x45, 0xc9, 0x69, 0x95, 0x53, 0xf9 } };

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
