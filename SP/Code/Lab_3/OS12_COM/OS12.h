#pragma once
#include <Unknwn.h>
#include <Windows.h>
	
// {90D312AB-BA71-4E48-BDF5-8C955DF847D4}
static const GUID IID_IAdder =
{ 0x90d312ab, 0xba71, 0x4e48, { 0xbd, 0xf5, 0x8c, 0x95, 0x5d, 0xf8, 0x47, 0xd4 } };

// {F109A160-79C4-4D31-ABC1-FB8C9911CC39}
static const GUID IID_IMultiplier =
{ 0xf109a160, 0x79c4, 0x4d31, { 0xab, 0xc1, 0xfb, 0x8c, 0x99, 0x11, 0xcc, 0x39 } };


// {712FACD1-7D85-4934-8134-583C8B1D7B71}
static const GUID CLSID_OS12 =
{ 0x712facd1, 0x7d85, 0x4934, { 0x81, 0x34, 0x58, 0x3c, 0x8b, 0x1d, 0x7b, 0x71 } };

__interface IAdder :public IUnknown {
	
	virtual HRESULT __stdcall Add(const double x, const double y, double& z) = 0;
	virtual HRESULT __stdcall Sub(const double x, const double y, double& z) = 0;
};

__interface IMultiplier :public IUnknown {
	virtual HRESULT __stdcall Mul(const double x, const double y, double& z) = 0;
	virtual HRESULT __stdcall Div(const double x, const double y, double& z) = 0;
};


//a function to create an instance of OS12 COM object(Used by MathFactory and implemented in OS12.cpp)
HRESULT CreateOS12Instance(REFIID riid, void** ppv);

extern LONG g_cObjects;
extern LONG g_cServerLocks;
