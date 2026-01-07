#pragma once
#pragma warning(disable:4996)
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <future>
#include <LM.h>
#pragma comment(lib,"netapi32.lib")


namespace HT {


	struct HTHANDLE {
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char HTUSerGroup[512], const char FileName[512]);
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
		char HTUserGroup[512];
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


	BOOL ValidateUserGroup(const char HTUserGroup[512]);

	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char HTUserGroup[512], const char FileName[512]);

	HTHANDLE* Open(const char FileName[512]);
	HTHANDLE* Open(const char HTUser[256], const char HTPassword[256], const char FileName[512]);

	BOOL Snap(HTHANDLE* handle);

	BOOL Close(HTHANDLE* handle);

	BOOL Insert(HTHANDLE* handle, Element* element);

	BOOL Delete(HTHANDLE* handle, Element* element);

	Element* Get(HTHANDLE* handle, Element* element);

	BOOL Update(HTHANDLE* handle, Element* element, const void* NewPayload, int NewPayloadLength);

	void Print(Element* element);



}