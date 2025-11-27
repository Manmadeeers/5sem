#pragma once
#include "Connector.h"

extern "C" {
	__declspec(dllexport) HRESULT CreateHTStorageInstance(REFIID riid, void** ppvObject);
}