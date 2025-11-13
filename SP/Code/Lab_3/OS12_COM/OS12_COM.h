#pragma once
#include <Windows.h>
#include <Unknwn.h>
#include <objbase.h>


// {25BE2623-37F3-419A-83D0-5B3EF389D854}
static const GUID IID_IAdder=
{ 0x25be2623, 0x37f3, 0x419a, { 0x83, 0xd0, 0x5b, 0x3e, 0xf3, 0x89, 0xd8, 0x54 } };

// {B2C1A043-23DF-41F9-850E-4DD29F6378EB}
static const GUID IID_IMultiplier =
{ 0xb2c1a043, 0x23df, 0x41f9, { 0x85, 0xe, 0x4d, 0xd2, 0x9f, 0x63, 0x78, 0xeb } };

// {260021D3-5E70-4163-8EF5-76A5536F0FB1}
static const GUID CLSID_OS12 =
{ 0x260021d3, 0x5e70, 0x4163, { 0x8e, 0xf5, 0x76, 0xa5, 0x53, 0x6f, 0xf, 0xb1 } };




struct IAdder :public IUnknown {
	virtual HRESULT _stdcall Add(const double x, const double y, double& z) = 0;
	virtual HRESULT _stdcall Sub(const double x, const double y, double& z) = 0;
};


struct IMultiplier :public IUnknown {
	virtual HRESULT _stdcall Mul(const double x, const double y, double& z) = 0;
	virtual HRESULT _stdcall Div(const double x, const double y, double& z) = 0;
};

