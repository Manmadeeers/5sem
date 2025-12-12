#pragma once
#include "Interface.h"

extern long g_cObjects = 0;
extern long g_cServerLocks = 0;

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

	OS13() :m_ref(1) {
		InterlockedIncrement(&g_cObjects);
	}

	virtual ~OS13() {
		InterlockedDecrement(&g_cObjects);
	}

private:
	volatile long m_ref;


};