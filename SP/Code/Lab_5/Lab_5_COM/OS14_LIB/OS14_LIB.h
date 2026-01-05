#pragma once
#include "../../Lab_5_COM/OS14_HTCOM/Interface.h"
#define OS14_HANDLE void*

namespace OS14_LIB {
	OS14_HANDLE Init();

	namespace OS14_HTCOM {
		HT::HTHANDLE* Create_HT(OS14_HANDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char UserGroup[512], const char FileName[512]);
		HT::HTHANDLE* Open_HT(OS14_HANDLE h, const char FileName[512]);
		HT::HTHANDLE* Open_HT(OS14_HANDLE h, const char User[256], const char Password[256], const char FileName[512]);
		BOOL Snap_HT(OS14_HANDLE h, HT::HTHANDLE* Handle);
		BOOL Close_HT(OS14_HANDLE h, HT::HTHANDLE* Handle);
		BOOL Insert_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		BOOL Delete_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		HT::Element* Get_HT(OS14_HANDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		void Print_HT(OS14_HANDLE h, HT::Element* Element);

		HT::Element* ConstructInsertElement_HT(OS14_HANDLE h, const void* Key, int KeyLength, const void* Payload, int PayloadLength);
		HT::Element* ConstructGetElement_HT(OS14_HANDLE h, const void* Key, int KeyLength);
		HT::Element* ConstructUpdateElement_HT(OS14_HANDLE h, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength);
	}

	void Dispose(OS14_HANDLE h);
}