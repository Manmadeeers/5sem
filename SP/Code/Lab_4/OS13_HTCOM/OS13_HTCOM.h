#pragma once
#include <Windows.h>
#include <Unknwn.h>
#include <iostream>
#include <atomic>
#include <mutex>
#include <string>
typedef unsigned int uint;

typedef struct HTHANDLE {
	int Capacity;
	int SecSnapshotInterval;
	int MaxKeyLength;
	int MaxPayloadLength;
	char FileName[512];
	HANDLE File;
	HANDLE FIleMapping;
	LPVOID Addr;
	int CurrentElements;
	HANDLE MutexHandle;
	char MutexName[256];
} HTHANDLE;

typedef struct ELEMENT {
	const void* Key;
	int KeyLength;
	const void* Payload;
	int PayloadLength;
}ELEMENT;

// {34AC9F7D-FB6C-492B-9760-B2B9EE1BE388}
static const GUID IID_IHTStorage =
{ 0x34ac9f7d, 0xfb6c, 0x492b, { 0x97, 0x60, 0xb2, 0xb9, 0xee, 0x1b, 0xe3, 0x88 } };

// {2EDF67BD-801C-4FD5-8598-1BD6769ED202}
static const GUID CLSID_HTCOM =
{ 0x2edf67bd, 0x801c, 0x4fd5, { 0x85, 0x98, 0x1b, 0xd6, 0x76, 0x9e, 0xd2, 0x2 } };


__interface IHTStorage :public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName,/*out*/HTHANDLE** ppHandle)PURE;
	virtual HRESULT STDMETHODCALLTYPE Open(const char* FileName, HTHANDLE** ppHandle)PURE;
	virtual HRESULT STDMETHODCALLTYPE Close(HTHANDLE* pHandle)PURE;
	virtual HRESULT STDMETHODCALLTYPE Insert(HTHANDLE* pHandle, const ELEMENT* Element)PURE;
	virtual HRESULT STDMETHODCALLTYPE Delete(HTHANDLE* pHandle, const ELEMENT* Element)PURE;
	virtual HRESULT STDMETHODCALLTYPE Update(HTHANDLE* pHandle, const ELEMENT* OldElement, const void* NewPayload, int NewPayloadLength)PURE;
	virtual HRESULT STDMETHODCALLTYPE Get(HTHANDLE* pHandle, const ELEMENT* Element,/*out*/ ELEMENT** ppElement)PURE;
	virtual HRESULT STDMETHODCALLTYPE Snap(HTHANDLE* pHandle)PURE;
	virtual HRESULT STDMETHODCALLTYPE Print(ELEMENT* Element)PURE;
};
