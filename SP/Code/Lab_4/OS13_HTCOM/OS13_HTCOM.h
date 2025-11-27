#pragma once
#include "pch.h"
#include <Windows.h>
#include <Unknwn.h>
#include <iostream>
#include <vector>


typedef unsigned int uint32;
typedef unsigned long uint64;


struct HTHandleInfo {
	uint32 Capacity;
	uint32 SecSnapshotInterval;
	uint32 MaxKeyLength;
	uint32 MaxPayloadLength;
	wchar_t FileName[512];
	uint32 CurrentElements;
	wchar_t CrossProcessMutexName[512];
};

struct ElementStruct {
	std::vector<BYTE>Key;
	uint32 KeyLength;
	std::vector<BYTE>Payload;
	uint32 PayloadLength;
};

struct ElementStructReturn {
	BYTE* Key;
	uint32 KeyLength;
	BYTE* Payload;
	uint32 PayloadLength;
};

// {38BD7D03-CB43-4CB1-BDE5-3FF8A8AAA2DB}
static const GUID IID_IHTStorage =	
{ 0x38bd7d03, 0xcb43, 0x4cb1, { 0xbd, 0xe5, 0x3f, 0xf8, 0xa8, 0xaa, 0xa2, 0xdb } };

// {94BB0DA0-E853-47B4-A6D7-6E960C65D314}
static const GUID IID_IHTHandle =
{ 0x94bb0da0, 0xe853, 0x47b4, { 0xa6, 0xd7, 0x6e, 0x96, 0xc, 0x65, 0xd3, 0x14 } };

// {AFB1C5EA-436E-4183-BEAA-91BC2371467B}
static const GUID CLSID_HTStorage =
{ 0xafb1c5ea, 0x436e, 0x4183, { 0xbe, 0xaa, 0x91, 0xbc, 0x23, 0x71, 0x46, 0x7b } };


__interface IHTHandle :public IUnknown {
	virtual HRESULT STDMETHODCALLTYPE Insert(const ElementStruct* element,/*out*/ BOOL* pbSuccess)PURE;
	virtual HRESULT STDMETHODCALLTYPE Delete(const ElementStruct* element, /*out*/BOOL* pbSuccess)PURE;
	virtual HRESULT STDMETHODCALLTYPE Get(const ElementStruct* searchElement,/*out*/ ElementStructReturn** ppFoundElement)PURE;
	virtual HRESULT STDMETHODCALLTYPE Update(const ElementStruct* oldElement, const BYTE* newPayload, uint32 newPayloadLength, /*out*/BOOL* pbSuccess)PURE;
	virtual HRESULT STDMETHODCALLTYPE Snap(/*out*/BOOL* pbSuccess)PURE;
	virtual HRESULT STDMETHODCALLTYPE Close(/*out*/BOOL* pbSuccess)PURE;
	virtual HRESULT STDMETHODCALLTYPE Print(const ElementStruct* element)PURE;
	virtual HRESULT STDMETHODCALLTYPE CreateSnapshotFileName(/*out*/BSTR* pSnapshotFileName)PURE;
	virtual HRESULT STDMETHODCALLTYPE GetHandleInfo(/*out*/HTHandleInfo* pInfo)PURE;
};


__interface IHTStorage :public IUnknown {

	virtual HRESULT STDMETHODCALLTYPE CrStorage(int Capacity,int SecSnapshotInterval,int MaxKeyLength,int MaxPayloadLength,const char FileName[512],IUnknown** ppHandle)PURE;
	virtual HRESULT STDMETHODCALLTYPE Open(const char FileName, IUnknown** ppHandle)PURE;
};



inline void FreeReturnedElement(ElementStructReturn* e) {
	if (!e) {
		return;
	}
	if (e->Key) {
		CoTaskMemFree(e->Key);
	}
	if (e->Payload) {
		CoTaskMemFree(e->Payload);
	}
	CoTaskMemFree(e);

}
