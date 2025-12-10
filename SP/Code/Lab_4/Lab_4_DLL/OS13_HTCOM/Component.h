#pragma once
#include "pch.h"
#include "HTInterface.h"
extern long g_cComponents;
extern long g_cServerLocks;



class OS13 :public IHT, public IElement {
public:
	//IUnknown
	virtual HRESULT __stdcall QueryInterface(REFIID riid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	//IHT
	STDMETHOD(COM_Create(HT::HTHANDLE** ppHandle, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName));
	STDMETHOD(COM_Open(HT::HTHANDLE** ppHandle, const char* FileName));
	STDMETHOD(COM_Snap(BOOL& rc, HT::HTHANDLE* Handle));
	STDMETHOD(COM_CLose(BOOL& rc, HT::HTHANDLE* Handle));
	STDMETHOD(COM_Insert(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element));
	STDMETHOD(COM_Delete(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* Element));
	STDMETHOD(COM_Get(HT::Element** ppElement, HT::HTHANDLE* Handle, HT::Element* Element));
	STDMETHOD(COM_Update(BOOL& rc, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength));
	STDMETHOD(COM_Print(HT::Element* Element));

	//IElement
	STDMETHOD(COM_ConstructGetElement(HT::Element** ppElement, const void* Key, int KeyLength));
	STDMETHOD(COM_ConstructInsertElement(HT::Element** ppElement, const void* Key, int KeyLength, const void* Payload, int PayloadLength));
	STDMETHOD(COM_ConstructUpdateElement(HT::Element** ppElement, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength));

	OS13() :m_ref(1) {
		InterlockedIncrement(&g_cComponents);
	}
	~OS13() {
		InterlockedDecrement(&g_cComponents);
		std::cout << "Component: Self destruction" << std::endl;
	}
private:
	volatile long m_ref;
};