#include "pch.h"

OS12::OS12() :counter(1) {}

OS12::~OS12() {}

HRESULT __stdcall OS12::QueryInterface(const IID& iid, void** ppv) {
	if (iid == IID_IADDER) {
		*ppv = (IAdder*)this;
	}
	else if (iid == IID_IMULTIPLIER) {
		*ppv = (IMultiplier*)this;
	}
	else {
		*ppv = this;
	}

	this->AddRef();
	return S_OK;
}


ULONG __stdcall OS12::AddRef() {
	InterlockedIncrement(&counter);
	return counter;
}

ULONG __stdcall OS12::Release() {
	InterlockedDecrement(&counter);

	if (counter == 0) {
		delete this;
		return 0;
	}

	return counter;
}

HRESULT __stdcall OS12::Add(const double x, const double y, double& z) {
	z = x + y;
	return S_OK;
}

HRESULT __stdcall OS12::Sub(const double x, const double y, double& z) {
	z = x - y;
	return S_OK;
}

HRESULT __stdcall OS12::Mul(const double x, const double y, double& z) {
	z = x * y;
	return S_OK;
}

HRESULT __stdcall OS12::Div(const double x, const double y, double& z) {
	z = x / y;
	return S_OK;
}