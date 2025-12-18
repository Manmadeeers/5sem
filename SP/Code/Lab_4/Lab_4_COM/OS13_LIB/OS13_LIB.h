#pragma once
#include "../../Lab_4_COM/OS13_HTCOM/Interface.h"
#define OS13_HANDLE void*


namespace OS13_LIB {
	OS13_HANDLE Init();
	namespace OS13_HTCOM {
		HT::HTHANDLE* Create_HT(OS13_HANDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		HT::HTHANDLE* Open_HT(OS13_HANDLE h, const char FileName[512]);
		BOOL Snap_HT(OS13_HANDLE h, HT::HTHANDLE* Handle);
		BOOL Close_HT(OS13_HANDLE h, HT::HTHANDLE* Handle);
		BOOL Insert_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		BOOL Update_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength);
		BOOL Delete_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		HT::Element* Get_HT(OS13_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		void Print_HT(OS13_HANDLE h, HT::Element* Element);
	}

	void Dispose(OS13_HANDLE h);
}