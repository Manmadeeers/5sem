#pragma once
#include "Interface.h"

//belongs to OS13.
extern long g_cComponents;
extern long g_cServerLocks;


class OS13 :public IHT {
public:
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	virtual HRESULT __stdcall COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
	virtual HRESULT __stdcall COM_Open(HT::HTHANDLE** ppHandle, const char FileName[512]);
	virtual HRESULT __stdcall COM_Snap(BOOL& rc, HT::HTHANDLE* Handle);
	virtual HRESULT __stdcall COM_Close(BOOL& rc, HT::HTHANDLE* Handle);
	virtual HRESULT __stdcall COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element);
	virtual HRESULT __stdcall COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element);
	virtual HRESULT __stdcall COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* ELement);
	virtual HRESULT __stdcall COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element, const void* NewPayload, int NewPayloadLength);
	virtual HRESULT __stdcall COM_Print(HT::Element* Element);
	virtual HRESULT __stdcall COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength);
	virtual HRESULT __stdcall COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength);
	virtual HRESULT __stdcall COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength);

	OS13() :m_ref(1) {
		InterlockedIncrement(&g_cComponents);
	}

	~OS13() {
		InterlockedDecrement(&g_cComponents);
		std::cout << "OS13: Destroying COM component" << std::endl;
	}

private:
	volatile long m_ref;


};