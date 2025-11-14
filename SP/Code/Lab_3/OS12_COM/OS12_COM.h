#pragma once
#include <objbase.h>
#include <Unknwn.h>
#include "IAdder.h"
#include "IMultiplier.h"


class OS12: public IAdder, public IMultiplier {
private:
	ULONG counter;
public:
	OS12();
	~OS12();
	HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	ULONG __stdcall AddRef();
	ULONG __stdcall Release();
	HRESULT __stdcall Add(const double, const double, double&);
	HRESULT __stdcall Sub(const double, const double, double&);
	HRESULT __stdcall Mul(const double, const double, double&);
	HRESULT __stdcall Div(const double, const double, double&);
};