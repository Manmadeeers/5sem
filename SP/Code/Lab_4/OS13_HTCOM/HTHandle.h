#pragma once
#include "Connector.h"

extern "C" {
	__declspec(dllexport)HRESULT CreateHTHandleInstance(HT::HTHANDLE* nativeHandle, IUnknown** ppHandle);
}