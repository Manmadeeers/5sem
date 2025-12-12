#pragma once
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <future>

namespace HT {

	struct HTHANDLE {
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		int Capacity;
		int SecSnapshotInterval;
		int MaxKeyLength;
		int MaxPayloadLength;
	    char FileName[512];
		HANDLE File;
		HANDLE FileMapping;
		LPVOID Addr;
		time_t LastSnapTime;
		int CurrentElements;
		HANDLE MutexHandle;
		char MutexName[512];
	};

	struct Element {
		Element();
		Element(const void* Key, int KeyLength);
		Element(const void* Key, int KeyLength, const void* Payload, int PayloadLength);
		Element(Element* OldElement, const void* NewPayload, int NewPayloadLength);
		const void* Key;
		int KeyLength;
		const void* Payload;
		int PayloadLength;
	};

	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);

	HTHANDLE* Open(const char FileName[512]);

	BOOL Snap(HTHANDLE* Handle);

	BOOL Close(HTHANDLE* Handle);

	BOOL Insert(HTHANDLE* Handle, Element* Element);

	BOOL Delete(HTHANDLE* Handle, Element* Element);

	Element* Get(HTHANDLE* Handle, Element* Element);

	BOOL Update(HTHANDLE* Handle, Element* OldElement, const void* NewPayload, int NewPayloadLength);

	void Print(Element* Element);
}