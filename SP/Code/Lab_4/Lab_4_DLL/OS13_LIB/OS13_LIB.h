#pragma once
#include "../OS13_HTCOM/HTInterface.h"
#define OS13_HTCOM_HAHNDLE void*


namespace OS13_LIB {
	OS13_HTCOM_HAHNDLE Init();

	namespace HT_LIB {
		HT::HTHANDLE* LIB_Create(OS13_HTCOM_HAHNDLE h, int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName);
		HT::HTHANDLE* LIB_Open(OS13_HTCOM_HAHNDLE h, const char* FileName);
		BOOL LIB_Snap(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle);
		BOOL LIB_Close(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle);
		BOOL LIB_Insert(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		BOOL LIB_Delete(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		HT::Element* LIB_Get(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* Element);
		BOOL LIB_Update(OS13_HTCOM_HAHNDLE h, HT::HTHANDLE* Handle, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength);
		void LIB_Print(OS13_HTCOM_HAHNDLE h, HT::Element* Element);
	}
	namespace ELEMENT_LIB {
		HT::Element* LIB_ConstructGetElement(OS13_HTCOM_HAHNDLE h, const void* Key, int KeyLength);
		HT::Element* LIB_ConstructInsertElement(OS13_HTCOM_HAHNDLE h, const void* Key, int KeyLength, const void* Payload, int PayloadLength);
		HT::Element* LIB_ConstructUpdateElement(OS13_HTCOM_HAHNDLE h, HT::Element* OldElement, const void* NewPayload, int NewPayloadLength);
	}
	
	void Dispose(OS13_HTCOM_HAHNDLE h);
}

