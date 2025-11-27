#pragma once
#include "OS13_HTCOM.h"
#include <atomic>
#include <mutex>
#include <comdef.h>
#include <strsafe.h>
#include <OS_11DLL.h>
#include "HTHandle.h"
#include "HTStorage.h"
#include "StorageFactory.h"

static long g_serverLocks = 0;
static std::mutex g_globalLock;

