#pragma once
#include "HTStorage.h"
#include <atomic>
#include <mutex>
#include <comdef.h>
#include <strsafe.h>


static long g_serverLocks = 0;
static std::mutex g_globalLock;

HRESULT CreateClassFactoryInstance(REFIID riid, void** ppv);