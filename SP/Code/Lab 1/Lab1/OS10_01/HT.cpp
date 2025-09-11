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
            throw new exception("FUCK file");
		}

        FileMapping = CreateFileMappingA(this->FileName, NULL, PAGE_READWRITE, 0, 0, NULL);

        if (FileMapping == INVALID_HANDLE_VALUE) {
            snprintf(LastErrorMessage, sizeof(LastErrorMessage), "Failed to create file.");
            throw new exception("FUCK file mapping");
        }

        Addr = MapViewOfFile(this->FileName, FILE_MAP_ALL_ACCESS, 0, 0, 0);
        if (Addr == INVALID_HANDLE_VALUE) {
            snprintf(LastErrorMessage, sizeof(LastErrorMessage), "Failed to create file.");
            throw new exception("FUCK map of view ");
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


    BOOL Insert(const HTHANDLE* hthandle, const Element* element) {
        std::lock_guard<std::mutex>lock(ht_mutex);

        if (!hthandle || !element || element->keylength > hthandle->MaxKeyLength || element->payloadlength > hthandle->MaxPayloadLength) {
            std::cout << "Failed to insert an element" << std::endl;
            return FALSE;
        }


    }

    //DOES NOT WORK 
    //FIX REQUIRED  
    HTHANDLE* Open(const char FileName[512]) {
        std::lock_guard<std::mutex> lock(ht_mutex);

        HTHANDLE* handle = new HTHANDLE();
        strncpy_s(handle->FileName, FileName, sizeof(handle->FileName));

        handle->File = CreateFileA(
            handle->FileName,
            GENERIC_READ | GENERIC_WRITE,
            0, // No sharing for this file
            NULL, // Set default security attributes
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (handle->File == INVALID_HANDLE_VALUE) {
            snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage),
                "Failed to open file: %s. Error code: %lu",
                FileName);
            delete handle;
            return nullptr;
        }

        handle->FileMapping = CreateFileMappingA(
            handle->File,
            NULL, // Default security
            PAGE_READWRITE,
            0,
            0,
            NULL
        );

        if (handle->FileMapping == NULL) {
            snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage),
                "Failed to create file mapping: %s",
                FileName);
            CloseHandle(handle->File);
            delete handle;
            return nullptr;
        }

        handle->Addr = MapViewOfFile(
            handle->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0, // Offset
            0, // Offset
            0  // Enable entire file usage
        );

        if (handle->Addr == NULL) {
            snprintf(handle->LastErrorMessage, sizeof(handle->LastErrorMessage),
                "Failed to map view of file: %s",
                FileName);
            CloseHandle(handle->FileMapping);
            CloseHandle(handle->File);
            delete handle;
            return nullptr;
        }

        return handle;
    }//does 
	
};