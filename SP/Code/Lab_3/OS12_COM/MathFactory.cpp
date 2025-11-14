#include "pch.h"
#include "MathFactory.h"


ULONG g_lObjs = 0;
ULONG g_lLocks = 0;

MathFactory::MathFactory() : m_lRef(1) {}

MathFactory::~MathFactory() {};

STDMETHODIMP_(HRESULT __stdcall) MathFactory::QueryInterface(REFIID riid, LPVOID* ppv)
{
    if (riid == IID_IUnknown)
        *ppv = (IUnknown*)this;
    else if (riid == IID_IClassFactory)
        *ppv = (IClassFactory*)this;
    else
        return E_NOINTERFACE;

    if (*ppv) {
        AddRef();
        return (S_OK);
    }
    else
        return (E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) MathFactory::AddRef() {
    InterlockedIncrement(&m_lRef);
    return m_lRef;
}

STDMETHODIMP_(ULONG) MathFactory::Release() {
    InterlockedDecrement(&m_lRef);
    if (m_lRef == 0)
    {
        delete this;
        return 0;
    }
    else
        return m_lRef;
}


STDMETHODIMP MathFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObj) {

    HRESULT hr = E_UNEXPECTED;

    OS12* pSP = nullptr;

    if (pUnkOuter != NULL)
        hr = CLASS_E_NOAGGREGATION;
    else if ((pSP = new OS12()) == NULL)
        hr = E_OUTOFMEMORY;
    else {
        hr = pSP->QueryInterface(riid, ppvObj);
        pSP->Release();
    }

    if (FAILED(hr))
        delete pSP;

    return hr;
}


STDMETHODIMP  MathFactory::LockServer(BOOL fLock) {
    if (fLock)
        InterlockedIncrement(&g_lLocks);
    else
        InterlockedDecrement(&g_lLocks);

    return S_OK;
}
