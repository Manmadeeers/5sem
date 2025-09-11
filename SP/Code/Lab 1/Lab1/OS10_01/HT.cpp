#include <iostream>
#include "OS10_1.h"
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>

using namespace std;

namespace HT {

	std::mutex ht_mutex;

	HTHANDLE::HTHANDLE() :
		Capacity(0),
		SecSnapshotInterval(0),
		MaxKeyLength(0),
		MaxPayloadLength(0),
		File{ 0 },
		FileMapping{ 0 },
		Addr{ nullptr },
		lastsnaptime(0) {
		memset(FileName, 0, sizeof(FileName));
		memset(LastErrorMessage, 0, sizeof(LastErrorMessage));
	}

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])

		: Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval),

		MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength), lastsnaptime(0) {

		strncpy_s(this->FileName, FileName, 512);

		memset(LastErrorMessage, 0, sizeof(LastErrorMessage));

		File = CreateFileA(this->FileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (File == INVALID_HANDLE_VALUE) {

			snprintf(LastErrorMessage, sizeof(LastErrorMessage), "Failed to create file.");
		}

	}

	Element::Element() :
		key(nullptr),
		keylength(0),
		payload(nullptr),
		payloadlength(0) {}

	Element::Element(const void* key, int keylength): key(key),keylength(keylength){}

	Element::Element(const void* key, int keylength, const void* payload, int  payloadlength):
		key(key),keylength(keylength),payload(payload),payloadlength(payloadlength){}

	HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {

		std::lock_guard<std::mutex>lock(ht_mutex);
		HTHANDLE* handle = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
		return handle;
	}

	HTHANDLE* Open(const char FileName[512]) {
		std::lock_guard<std::mutex>lock(ht_mutex);
		HTHANDLE* handle = new HTHANDLE();
		handle->File = CreateFileA(FileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (handle->File == INVALID_HANDLE_VALUE) {
			snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage), "Failed to open a file");
			delete handle;
			return nullptr;
		}

		handle->FileMapping = CreateFileMapping(handle->File, NULL, PAGE_READWRITE, 0, handle->Capacity * sizeof(Element), NULL);

		if (!handle->FileMapping) {
			snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage), "Failed to create file mapping");
			CloseHandle(handle->File);
			delete handle;
			return nullptr;
;		}

		handle->Addr = MapViewOfFile(handle->FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (!handle->Addr) {
			snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage), "Failed to map view of file");
			CloseHandle(handle->FileMapping);
			CloseHandle(handle->File);
			delete handle;
			return nullptr;
		}

		return handle;


	}

	BOOL Snap(const HTHANDLE* hthandle) {
		std::lock_guard<std::mutex> lock(ht_mutex);

		//TODO:implement
		return TRUE;


	}

	BOOL Close(const HTHANDLE* hthandle) {
		std::lock_guard<std::mutex> lock(ht_mutex);
		UnmapViewOfFile(hthandle->Addr);
		CloseHandle(hthandle->FileMapping);
		CloseHandle(hthandle->File);
		return TRUE;
	}

	BOOL Insert(const HTHANDLE* hthandle, const Element* element) {
		std::lock_guard<std::mutex> lock(ht_mutex);

		int index = std::hash<std::string>{}(std::string(static_cast<const char*>(element->key), element->keylength)) % hthandle->Capacity;


		for (int i = 0; i < hthandle->Capacity; ++i) {

			int probeIndex = (index + i) % hthandle->Capacity;

			Element* slot = reinterpret_cast<Element*>(

				static_cast<char*>(hthandle->Addr) + probeIndex * sizeof(Element));

			if (slot->key == nullptr) {

				memcpy(slot, element, sizeof(Element));

				return TRUE;

			}

		}

		snprintf(const_cast<char*>(hthandle->LastErrorMessage), sizeof(hthandle->LastErrorMessage), "HT storage is full.");

		return FALSE;
	}

	BOOL Delete(const HTHANDLE* hthandle, const Element* element) {
		std::lock_guard<std::mutex> lock(ht_mutex);
		//TODO:implement
		return true;
	}

	Element* Get(const HTHANDLE* hthandle, const Element* element) {
		std::lock_guard<std::mutex> lock(ht_mutex);
		//TODO:implement;

		return nullptr;//to be changed to a real element that should be returned
	}

	BOOL Update(const HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength) {
		std::lock_guard<std::mutex> lock(ht_mutex);
		//TODO:implement

		return TRUE;
	}

	char* GetLastError(HTHANDLE*ht) {
		std::lock_guard<std::mutex> lock(ht_mutex);
		return ht->LastErrorMessage;
	}

	void print(const Element* element) {
		std::cout << "Key: " << static_cast<const char*>(element->key) << ", Payload: " << static_cast<const char*>(element->payload) << std::endl;
	}
};