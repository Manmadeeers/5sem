#pragma once
#include "Connector.h"

extern "C" {
	__declspec(dllexport) HRESULT CreateStorageFactoryInstance(REFIID riid, void**ppv);
}