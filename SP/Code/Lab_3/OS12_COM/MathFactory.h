#pragma once
#include <Unknwn.h>

class MathFactory : public IClassFactory {
private:
	ULONG m_lRef;
public:
	MathFactory();
	~MathFactory();

	STDMETHOD(QueryInterface(REFIID riid, LPVOID* ppv));
	STDMETHOD_(ULONG, AddRef(void));
	STDMETHOD_(ULONG, Release(void));

	STDMETHOD(CreateInstance(IUnknown* pUO, const IID& iid, LPVOID* ppv));
	STDMETHOD(LockServer(BOOL b));
};