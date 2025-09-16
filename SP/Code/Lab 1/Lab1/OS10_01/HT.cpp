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

        int storage_size = ht->Capacity * (ht->MaxKeyLength + ht->MaxPayloadLength);
       
        ht->FileMapping = CreateFileMappingA(
            ht->File, // Handle to the file
            NULL, //security descriptor: Default security descriptor
            PAGE_READWRITE, //access mode: read & write
            0, // Maximum size (higher DWORD) - responsible for more 4GB files
            storage_size, // Maximum size (lower DWORD) responsible for less 4GB files
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


    BOOL Snap(const HTHANDLE* hthandle) {

        cout << endl << "----------Snap----------" << endl;

        if (!hthandle) {
            cout << "--Snap:Failed to open the handle--"<<"Error: "<<GetLastError() << endl;
            return FALSE;
        }

        HANDLE HTSnapshot = CreateFileA(
            "TestSnapshot.htsnap",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );


        if (HTSnapshot == INVALID_HANDLE_VALUE) {
            cout << "--Snap:Failed to create a snapshot file--"<<"Error: "<<GetLastError() << endl;
            return FALSE;
        }
        else {
            cout << "--Snap:Snapshot file created--" << endl;
        }

        SIZE_T data_size = hthandle->CurrentElements * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength);
        cout << "--Data size to write: " << data_size << "--" << endl;

        cout << "--Buffer size: " << sizeof(hthandle->Addr) << "--" << endl;

        DWORD bytesWritten;

        BOOL writeResult = WriteFile(
            HTSnapshot,
            hthandle->Addr,
            data_size,
            &bytesWritten,
            NULL
        );

        if (!writeResult) {
            DWORD writeEror = GetLastError();
            cout << "--Snap:Failed to execute a snapshot(WriteFile error)--" << " Error: " << writeEror << endl;
            cout << endl << "----------End----------" << endl;
            CloseHandle(HTSnapshot);
            return FALSE;
        }
        else if (bytesWritten != data_size) {
            DWORD bytesError = GetLastError();
            cout << "--Snap:failed to execute a snapshot(Bytes loss)--" << " Error: " << bytesError << endl;
            cout << endl << "----------End----------" << endl;
            CloseHandle(HTSnapshot);
            return FALSE;
        }
        else {
            cout << "--Snap: Snapshot Executed--" << endl;
        }

        CloseHandle(HTSnapshot);
        return TRUE;
        cout << endl << "----------End----------" << endl;

    }

    BOOL Close(const HTHANDLE* hthandle) {

        if (!hthandle) {
            cout << "--Close:Failed To Close(Invalid handle)--"<<" Error: "<<GetLastError() << endl;
            return FALSE;
        }

        
        if (Snap(hthandle)) {
            cout << "--Close:Snapshot taken--" << endl;
        }
        else {
            cout << "--Closing:Failed To Take a Snapshot--"<<" Error: "<<GetLastError() << endl;
            return FALSE;
        }

        if (hthandle->Addr != NULL) {
            UnmapViewOfFile(hthandle->Addr);
            cout << "--Close: Unmapped View Of File--" << endl;
        }
        if (hthandle->FileMapping != NULL) {
            CloseHandle(hthandle->FileMapping);
            cout << "--Close: File Mapping Handle Closed--" << endl;
        }
        if (hthandle->File != NULL) {
            BOOL result = CloseHandle(hthandle->File);
            if (!result) {
                cout << "--Close:Failed To Close The File Handle--"<<GetLastError() << endl;
                return FALSE;
            }
        }
        cout << "--Close:File Handle Closed Successfully--" << endl;
        return TRUE;
    }

    //WORKS PROPERLY BUT THE INSERTION PROCESS NEEDS TO BE REWISED
    BOOL Insert(HTHANDLE* hthandle, const Element* element) {

        if (hthandle == NULL) {
            cout << "--Insert: Failed to insert(Invalid handle)--" << " Error: " << GetLastError() << endl;
            return FALSE;
        }
        else if (element == NULL) {
            cout << "--Insert: Failed to insert(Invalid element)--" << " Error: " << GetLastError() << endl;
            return FALSE;
        }
        else if (hthandle->Addr == NULL) {
            cout << "--Insert: Failed to insert(Address was null)" << endl;
            return FALSE;
        }

        if (hthandle->CurrentElements >= hthandle->Capacity) {
            cout << "--Insert: Failed to insert(attempted to exceed the handle capacity)--" << endl;
            return FALSE;
        }

        if (element->keylength > hthandle->MaxKeyLength) {
            cout << "--Insert: Failed to insert(Element's key length is too big)--" << endl;
            return FALSE;
        }
        
        if (element->payloadlength > hthandle->MaxPayloadLength) {
            cout << "--Insert: Failed to insert(Element's payload length is too big)--" << endl;
            return FALSE;
        }

        int next_index = hthandle->CurrentElements;

        char* storage_location = static_cast<char*>(hthandle->Addr) + (next_index * (hthandle->MaxKeyLength + hthandle->MaxPayloadLength));

        if (element->keylength > 0) {
            memcpy(storage_location, element->key, element->keylength);
        }
        else {
            cout << "--Insert: Failed to insert an element with 0 key length--" << endl;
            return FALSE;
        }

        if (element->payloadlength > 0) {
            memcpy(storage_location + element->keylength, element->payload, element->payloadlength);
        }
        else {
            cout << "--Insert: Failed to insert an element with 0 payload length--" << endl;
        }

        hthandle->CurrentElements += 1;
        cout << "--Insert: Element inserted successfully--" << endl;
        return TRUE;
    }


    BOOL Delete(HTHANDLE* handle, const Element* element) {
        if (handle == NULL||handle->Addr==NULL) {
            cout << "--Delete: Failed to delete an element(handle was invalid)--" << endl;
            return FALSE;
        }

        if (element == NULL || element->keylength == NULL || element->payloadlength == NULL) {
            cout << "--Delete: Failed to delete an element(element was invalid)--" << endl;
            return FALSE;
        }


        int index_to_delete = -1;

        for (int i = 0; i < handle->CurrentElements; ++i) {
            char* storage_location = static_cast<char*>(handle->Addr)+(i*(handle->MaxKeyLength+handle->MaxPayloadLength));

            if (memcmp(storage_location, element->key, element->keylength) == 0) {
                index_to_delete = i;
                break;
            }
        }

        if (index_to_delete == -1) {
            cout << "--Delete: Failed to delete an element(element not found)--" << endl;
            return FALSE;
        }

        for (int i = index_to_delete + 1; i < handle->CurrentElements; ++i) {
            char* src_location = static_cast<char*>(handle->Addr) + (i * (handle->MaxKeyLength + handle->MaxPayloadLength));
            char* dest_location = static_cast<char*>(handle->Addr) + ((i - 1) * (handle->MaxKeyLength + handle->MaxPayloadLength));
            memcpy(dest_location, src_location, handle->MaxKeyLength + handle->MaxPayloadLength);
        }


        handle->CurrentElements -= 1;
        cout << "Delete: Successfully deleted an element with key: " << element->key << "--" << endl;
        return TRUE;

    }




	
};