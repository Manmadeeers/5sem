#include <iostream>
#include "OS10_1.h"
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>

using namespace std;

namespace HT {

	std::mutex ht_mutex;

    //default constructor
    Element::Element() :
        key(nullptr),
        keylength(0),
        payload(nullptr),
        payloadlength(0){}

    //for Get operation
    Element::Element(const void*key, int keylength):
        key(key),
        keylength(keylength),
        payload(nullptr),
        payloadlength(0){}

    //for Insert operation
    Element::Element(const void* key, int keylength, const void* payload, int  payloadlength):
        key(key),
        keylength(keylength),
        payload(payload),
        payloadlength(payloadlength){}

    //for Update operation
    Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength):
        key(oldelement->key),
        keylength(oldelement->keylength),
        payload(newpayload),
        payloadlength(newpayloadlength){ }




    HTHANDLE::HTHANDLE() :
        Capacity(0),
        SecSnapshotInterval(0),
        MaxKeyLength(0),
        MaxPayloadLength(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        lastsnaptime(0),
        CurrentElements(0)
    {
        
	}

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])

		: Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval),

		MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength), lastsnaptime(0),
        File(NULL),FileMapping(NULL),Addr(NULL),CurrentElements(0)
    {

        strcpy_s(this->FileName, 512, FileName);

       

	}

    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {
        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength,FileName);

        
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,//access mode:read &writes
            0, //sharing between processes: No sharing
            NULL, //security attributes: Default security attributes    
            OPEN_ALWAYS, //how to open: Open if exists, else create
            FILE_ATTRIBUTE_NORMAL,//file attribute:normal
            NULL // No template file
        );

        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "--File Creation Failed(Create)-- Error: " << GetLastError() << endl;
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Creation Successful(Create)--" << endl;
        }

        DWORD fileSize = GetFileSize(ht->File, NULL);
        if (fileSize == INVALID_FILE_SIZE) {
            cout << "File error: " << GetLastError() << endl;
        }
       
        ht->FileMapping = CreateFileMappingA(
            ht->File, // Handle to the file
            NULL, //security descriptor: Default security descriptor
            PAGE_READWRITE, //access mode: read & write
            0, // Maximum size (higher DWORD) - responsible for more 4GB files
            0x00000001, // Maximum size (lower DWORD) responsible for less 4GB files
            "HtMapping" //named mapping(null if not named)
        );
            
        if (ht->FileMapping == NULL) {
            DWORD error = GetLastError();
            cout << "--File Mapping Failed(Create)-- Error: " << error << endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Mapping Successful(Create)--" << endl;
        }

        
        ht->Addr = MapViewOfFile(
            ht->FileMapping,//handle to file mappint
            FILE_MAP_ALL_ACCESS, //access mode: all access
            0, // Offset high: from the top of the file
            0, // Offset low: from the bottom of the file
            0 // Map the entire file
        );

        if (ht->Addr == NULL) {
            cout << "--Failed to Map View Of File(Create)-- Error: " << GetLastError() << endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--MapViewOfFile successful(Create)--" << endl;
        }

        return ht;
    }


    HTHANDLE* Open(const char FileName[512]) {
        HTHANDLE* ht = new HTHANDLE();
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,//this is the only difference in a parameters of CreateFileA function. here we hope that there is a file with FileName and try to open it
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (ht->File == INVALID_HANDLE_VALUE) {
            cout << "--File Creation Failed(Open)--" << endl;
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Creation Successful(Open)--" << endl;
        }
        ht->FileMapping = CreateFileMappingA(
            ht->File,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            "SharedHTMapping"
        );
        if (ht->FileMapping == NULL) {
            cout << "--File Mapping Failed(Open)--" << endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--File Mapping Successful(Open)--" << endl;
        }

        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );
        if (ht->Addr == NULL) {
            cout << "--Map View Of File Failed(Open)--" << endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            cout << "--Map View Of File Successful(Open)--" << endl;
        }
        return ht;
    }
    
    //IN PROGRESS
    BOOL Snap(const HTHANDLE* hthandle) {

        cout << endl << "----------Snap----------" << endl;

        if (!hthandle) {
            cout << "--Failed to open the handle--" << endl;
            return FALSE;
        }

        HANDLE HTSnapshot = CreateFileA(
            "HtSnapshot.htsnap",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
            

        cout << endl << "----------End----------" << endl;

    }

    BOOL Close(const HTHANDLE* hthandle) {

        if (!hthandle) {
            cout << "--Failed To Close. Hthandle was NULL--" << endl;
            return FALSE;
        }

        //executing a snapshot before closing an hthandle0
        if (Snap(hthandle)) {
            cout << "--Closing...Snapshot taken--" << endl;
        }
        else {
            cout << "--Closing...Failed To Take a Snapshot--" << endl;
            return FALSE;
        }

        if (hthandle->Addr != NULL) {
            UnmapViewOfFile(hthandle->Addr);//taking FileMapping out of the RAM
            cout << "--Unmapped View Of File--" << endl;
        }
        if (hthandle->FileMapping != NULL) {
            CloseHandle(hthandle->FileMapping);
            cout << "--File Mapping Handle Closed--" << endl;
        }
        if (hthandle->File != NULL) {
            BOOL result = CloseHandle(hthandle->File);
            if (!result) {
                cout << "--Failed To Close The File Handle--" << endl;
                return FALSE;
            }
        }
        cout << "--File Handle Closed Successfully--" << endl;
        return TRUE;
    }


    BOOL Insert(HTHANDLE* hthandle, const Element* element) {

        if (hthandle == NULL || hthandle->Addr == NULL || element == NULL) {
            cout << "--Failed To Insert Element. Invalid Handle Or Element--" << endl;
            return FALSE;
        }


        if (hthandle->CurrentElements >= hthandle->Capacity) {
            cout << "--Could Not Insert a New Element Because It Exceeds The Boundaries Of The Storage--" << endl;
            return FALSE;
        }

        int nextIndex = hthandle->CurrentElements;

        char* storageLocation = static_cast<char*>(hthandle->Addr) + (nextIndex * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength));

        if (element->keylength > 0) {
            memcpy(storageLocation,element->key,element->keylength );
        }
        else {
            cout << "--Failed To Insert an Element With an Empty Key--" << endl;
            return FALSE;
        }

        if (element->payloadlength > 0) {
            memcpy(storageLocation + hthandle->MaxKeyLength, element->payload, element->payloadlength);
        }
        else {
            cout << "--Failed To Insert an Element With an Empty Payload--" << endl;
            return FALSE;
        }

        hthandle->CurrentElements += 1;
       cout << "--Element Inserted Successfully--" << endl;

       return TRUE;
    }




	
};