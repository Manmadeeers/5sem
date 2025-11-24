#pragma once
#include <Windows.h>
#include <Unknwn.h>

// {38BD7D03-CB43-4CB1-BDE5-3FF8A8AAA2DB}
static const GUID IID_IHTStorage =
{ 0x38bd7d03, 0xcb43, 0x4cb1, { 0xbd, 0xe5, 0x3f, 0xf8, 0xa8, 0xaa, 0xa2, 0xdb } };

__interface IHTStorage :public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE Create(BSTR fileName, ULONG capacity, ULONG secSnapshotInterval, ULONG maxKeyLength, ULONG maxPayloadLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE Open(BSTR fileName) = 0;
	virtual HRESULT STDMETHODCALLTYPE Snap() = 0;
	virtual HRESULT STDMETHODCALLTYPE Close() = 0;
	virtual HRESULT STDMETHODCALLTYPE Insert(BYTE* key, ULONG keyLangth, BYTE* payload, ULONG payloadLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE Delete(BYTE* key, ULONG keyLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE Get(BYTE* key, ULONG keyLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE Update(BYTE* key, ULONG keyLength, BYTE* newPayload, ULONG newPayloadLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE Print(BYTE* key, ULONG keyLength) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateSnapshotFileName(BSTR* fileName) = 0;
};