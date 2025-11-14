#pragma once
#include <objbase.h>
#include <Unknwn.h>

// {6BB44CE1-88B5-40D6-ADFD-6C1B0FBA8255}
static const CLSID IID_IADDER = { 0x6bb44ce1, 0x88b5, 0x40d6, { 0xad, 0xfd, 0x6c, 0x1b, 0xf, 0xba, 0x82, 0x55 } };

__interface IAdder:IUnknown {
	virtual HRESULT __stdcall Add(const double x, const double y, double& z) = 0;
	virtual HRESULT __stdcall Sub(const double x, const double y, double& z) = 0;
};