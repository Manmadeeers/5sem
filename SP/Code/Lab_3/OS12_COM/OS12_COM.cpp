#include "OS12_COM.h"
#include <iostream>
#include <atomic>
#include <Windows.h>
#include <objbase.h>
#include <strsafe.h>


std::atomic<ULONG> g_objectCount(0);
std::atomic<ULONG>g_lockCount(0);
HINSTANCE g_hInst = NULL;

class OS12Object :public IAdder, public IMultiplier {
	
public:
		OS12Object() :m_ref(1) { ++g_objectCount; }
		
};

