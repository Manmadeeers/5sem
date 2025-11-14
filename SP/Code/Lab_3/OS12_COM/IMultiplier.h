#pragma once
#include <objbase.h>
#include <Unknwn.h>

// {8CEE6B5E-74D1-49C3-84AE-11F2BF85D466}
static const CLSID IID_IMULTIPLIER =
{ 0x8cee6b5e, 0x74d1, 0x49c3, { 0x84, 0xae, 0x11, 0xf2, 0xbf, 0x85, 0xd4, 0x66 } };

__interface IMultiplier:IUnknown
{
	virtual HRESULT __stdcall Mul(const double x, const double y, double& z) = 0;
	virtual HRESULT __stdcall Div(const double x, const double y, double& z) = 0;
};

