#include <iostream>
#include <Unknwn.h>
#include "../OS12_COM/IAdder.h"
#include "../OS12_COM/IMultiplier.h"
#define IERR(s) std::cout << "error: " << s << std::endl;
#define IRES(s,r) std::cout << s << r << std::endl;

IAdder* pIAdder = nullptr;
IMultiplier* pIMultiplier = nullptr;

// {8A0AD2AD-7C3A-485C-98F4-EA7520B1FAAD}
static const CLSID CLSID_CA =
{ 0x8a0ad2ad, 0x7c3a, 0x485c, { 0x98, 0xf4, 0xea, 0x75, 0x20, 0xb1, 0xfa, 0xad } };


void main() {
	IUnknown* pIUnknown = nullptr;

	CoInitialize(NULL);

	HRESULT hr0 = CoCreateInstance(CLSID_CA, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown);

	if (FAILED(hr0)) {
		IERR("CoCreateInstance");
		return;
	}

	if (SUCCEEDED(pIUnknown->QueryInterface(IID_IMULTIPLIER, (void**)&pIMultiplier))) {
		double z = 0.0;

		if (FAILED(pIMultiplier->Mul(2.0, 3.0, z))) {
			IERR("IMultiplier->Mul")
		}
		else {
			IRES("2 * 3 = ", z)
		}

		if (FAILED(pIMultiplier->Div(6.0, 3.0, z))) {
			IERR("IMultiplier->Div")
		}
		else {
			IRES("6 / 3 = ", z)
		}
	}
	else {
		IERR("IMultiplier->QueryInterface")
	}

	pIMultiplier->Release();

	if (SUCCEEDED(pIUnknown->QueryInterface(IID_IADDER, (void**)&pIAdder))) {
		double z = 0.0;

		if (FAILED(pIAdder->Add(2.0, 3.0, z))) {
			IERR("IAdder->Add")
		}
		else {
			IRES("2 + 3 = ", z)
		}

		if (FAILED(pIAdder->Sub(6.0, 3.0, z))) {
			IERR("IAdder->Sub")
		}
		else {
			IRES("6 - 3 = ", z)
		}
	}
	else {
		IERR("IAdder->QueryInterface")
	}

	pIAdder->Release();
	pIUnknown->Release();

	CoUninitialize();
}
