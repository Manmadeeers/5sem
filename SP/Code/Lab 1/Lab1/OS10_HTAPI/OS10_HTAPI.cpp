#include "pch.h"
#include "framework.h"
#include "OS_10_2.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <Windows.h>
#include <cstdlib>

using namespace std;


namespace HT {

    std::mutex ht_mutex;

    //default constructor
    Element::Element() :
        key(nullptr),
        keylength(0),
        payload(nullptr),
        payloadlength(0) {
    }

    //for Get operation
    Element::Element(const void* key, int keylength) :
        key(key),
        keylength(keylength),
        payload(nullptr),
        payloadlength(0) {
    }

    //for Insert operation
    Element::Element(const void* key, int keylength, const void* payload, int  payloadlength) :
        key(key),
        keylength(keylength),
        payload(payload),
        payloadlength(payloadlength) {
    }

    //for Update operation
    Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength) :
        key(oldelement->key),
        keylength(oldelement->keylength),
        payload(newpayload),
        payloadlength(newpayloadlength) {
    }




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
        File(NULL), FileMapping(NULL), Addr(NULL), CurrentElements(0)
    {

        strcpy_s(this->FileName, 512, FileName);



    }

    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {
        lock_guard<mutex>lock(ht_mutex);

        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
        cout << "----------Creation Started----------" << endl << endl;



        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,//access mode:read &writes
            0, //sharing between processes: No sharing
            NULL, //security attributes: Default security attributes    
            CREATE_ALWAYS, //how to open: Open if exists, else create
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


        int slot_size = (ht->MaxKeyLength + ht->MaxPayloadLength);
        cout << "Slot size: " << slot_size << endl;

        int metadata_offset = 3 * sizeof(int);
        cout << "Metadata offset: " << metadata_offset << endl;

        cout << "Storage capacity: " << ht->Capacity << endl;
        int storage_size = metadata_offset + (ht->Capacity * slot_size);
        cout << "Storage size: " << storage_size << endl;


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
            ht->FileMapping,//handle to file mapping
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


        memcpy(ht->Addr, &Capacity, sizeof(int));

        memcpy(static_cast<char*>(ht->Addr) + sizeof(int), &MaxKeyLength, sizeof(int));

        memcpy(static_cast<char*>(ht->Addr) + 2 * sizeof(int), &MaxPayloadLength, sizeof(int));


        //ht->Addr = static_cast<char*>(ht->Addr) + metadata_offset;


        return ht;
    }


    HTHANDLE* Open(const char FileName[512]) {
        cout << "----------Opening Started----------" << endl << endl;

        lock_guard<mutex>lock(ht_mutex);
        HTHANDLE* ht = new HTHANDLE();
        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
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

        memcpy(&ht->Capacity, ht->Addr, sizeof(int));
        memcpy(&ht->MaxKeyLength, static_cast<char*>(ht->Addr) + sizeof(int), sizeof(int));
        memcpy(&ht->MaxPayloadLength, static_cast<char*>(ht->Addr) + 2 * sizeof(int), sizeof(int));

        cout << "Current ht->Addr: " << ht->Addr << endl;

        int slot_size = ht->MaxKeyLength + ht->MaxPayloadLength;
        cout << "Computed slot size: " << slot_size << endl;

        int metadata_offset = 3 * sizeof(int);

        /* ht->Addr = static_cast<char*>(ht->Addr) + metadata_offset;*/
        cout << "Computed ht->Addr: " << ht->Addr << endl;

        int total_mem = slot_size * ht->Capacity;
        cout << "Total mem: " << total_mem << endl;


        cout << "Opened HT storage has " << ht->Capacity << " capacity, " << ht->MaxKeyLength << " Max key length, " << ht->MaxPayloadLength << " max payload length" << endl;

        return ht;
    }

    const char* CreateSnapshotFileName(HTHANDLE* handle) {
        static char buffer[100];
        char time_buffer[80];
        tm time_info;

        localtime_s(&time_info, &handle->lastsnaptime);
        strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &time_info);
        snprintf(buffer, sizeof(buffer), "Snapshot-%s.htsnap", time_buffer);

        return buffer;

    }

    BOOL Snap(HTHANDLE* hthandle) {

        cout << endl << "----------Snap----------" << endl;

        if (!hthandle) {
            cout << "--Snap:Failed to open the handle--" << "Error: " << GetLastError() << endl;
            return FALSE;
        }

        //not needed here if the snapshot is being executed during closure procedure
        //lock_guard<mutex>lock(ht_mutex)

       /* OVERLAPPED ol = { 0 };
        ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);*/

        hthandle->lastsnaptime = time(nullptr);

        HANDLE HTSnapshot = CreateFileA(
            CreateSnapshotFileName(hthandle),
            //"Snapshotfile.htsnap",
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );


        if (HTSnapshot == INVALID_HANDLE_VALUE) {
            cout << "--Snap:Failed to create a snapshot file--" << "Error: " << GetLastError() << endl;
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
        //CloseHandle(ol.hEvent);
        return TRUE;
        cout << endl << "----------End----------" << endl;

    }

    BOOL Close(HTHANDLE* hthandle) {

        if (!hthandle) {
            cout << "--Close:Failed To Close(Invalid handle)--" << " Error: " << GetLastError() << endl;
            return FALSE;
        }

        lock_guard<mutex>lock(ht_mutex);

        if (Snap(hthandle)) {
            cout << "--Close:Snapshot taken--" << endl;
        }
        else {
            cout << "--Closing:Failed To Take a Snapshot--" << " Error: " << GetLastError() << endl;
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
                cout << "--Close:Failed To Close The File Handle--" << GetLastError() << endl;
                return FALSE;
            }
        }
        cout << "--Close:File Handle Closed Successfully--" << endl;
        return TRUE;
    }


    //using classical DJB2 algorithm

    int hashFunction(const void* key, int keyLength, int capacity) {
        int hash = 5381;


        const char* str = static_cast<const char*>(key);

        for (int i = 0; i < keyLength; ++i) {


            hash = ((hash << 5) + hash) + str[i];

        }

        cout << "--Hash: current Hash value: " << hash << endl;


        return abs(hash % capacity);
    }


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
        if (element->keylength == NULL) {
            cout << "--Insert: Failed to insert(key length was NULL)" << endl;
            return FALSE;
        }
        if (element->payloadlength == NULL) {
            cout << "--Insert: Failed to insert(payload length was NULL)" << endl;
            return FALSE;
        }

        lock_guard<mutex> lock(ht_mutex);

        const int metadata_offset = 3 * sizeof(int);
        const int slot_size = hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
        int hash_index = hashFunction(element->key, element->keylength, hthandle->Capacity);

        cout << "--Insert: initial hash index: " << hash_index << endl;

        char* base = static_cast<char*>(hthandle->Addr) + metadata_offset;


        for (int probe = 0; probe < hthandle->Capacity; ++probe) {
            char* slot_key = base + (hash_index * slot_size);

            bool is_empty = true;

            for (int k = 0; k < hthandle->MaxKeyLength; ++k) {
                if (slot_key[k] != 0) { is_empty = false; break; }
            }

            if (is_empty) {

                memcpy(slot_key, element->key, element->keylength);
                if (element->keylength < hthandle->MaxKeyLength) {
                    memset(slot_key + element->keylength, 0, hthandle->MaxKeyLength - element->keylength);
                }

                char* payload_area = slot_key + hthandle->MaxKeyLength;
                memcpy(payload_area, element->payload, element->payloadlength);
                if (element->payloadlength < hthandle->MaxPayloadLength) {
                    memset(payload_area + element->payloadlength, 0, hthandle->MaxPayloadLength - element->payloadlength);
                }

                hthandle->CurrentElements++;
                cout << "--Insert: inserted at index " << hash_index << endl;
                return TRUE;
            }

            hash_index = (hash_index + 1) % hthandle->Capacity;
        }

        cout << "--Insert: no free slot found--" << endl;
        return FALSE;
    }


    BOOL Delete(HTHANDLE* handle, const Element* element) {


        cout << endl << "----------Deletion Started----------" << endl << endl;
        if (handle == NULL || handle->Addr == NULL) {
            cout << "--Delete: Failed to delete an element(handle was invalid)--" << endl;
            return FALSE;
        }

        if (element == NULL || element->keylength == NULL) {
            cout << "--Delete: Failed to delete an element(element was invalid)--" << endl;
            return FALSE;
        }

        std::lock_guard<std::mutex> lock(ht_mutex);

        const size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        const int metadata_offset = 3 * sizeof(int);
        char* base = static_cast<char*>(handle->Addr) + metadata_offset;

        int index_to_delete = -1;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);

            bool slot_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; ++k) {
                if (current_slot[k] != 0) {
                    slot_empty = false;
                    break;
                }
            }
            if (slot_empty) {
                continue;
            }

            if (element->keylength <= handle->MaxKeyLength && memcmp(current_slot, element->key, element->keylength) == 0) {
                index_to_delete = i;
                break;
            }
        }

        if (index_to_delete == -1) {
            cout << "--Delete: Failed to delete an element (element not found)--" << endl;
            return FALSE;
        }

        for (int i = index_to_delete + 1; i < handle->CurrentElements; ++i) {
            char* src_location = base + (i * slot_size);
            char* dest_location = base + ((i - 1) * slot_size);
            memcpy(dest_location, src_location, slot_size);
        }


        handle->CurrentElements--;

        cout << "Delete: Successfully deleted an element with key: " << static_cast<const char*>(element->key) << endl;
        cout << "----------Deletion Ended----------" << endl;

        return TRUE;
    }

    Element* Get(const HTHANDLE* handle, const Element* element) {

        if (handle == NULL || handle->Addr == NULL) {
            cout << "--Get: Failed to get an element(handle was invalid)--" << endl;
            return NULL;
        }

        if (element == NULL || element->keylength == NULL) {
            cout << "--Get: Failed to get an element(element was invalid)--" << endl;
            return NULL;
        }
        lock_guard<mutex>lock(ht_mutex);
        int offset = 3 * sizeof(int);
        size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + offset;

        for (int i = 0; i < handle->Capacity; ++i) {

            char* current_slot = base + (i * slot_size);

            bool is_empty = true;

            for (int j = 0; j < handle->MaxKeyLength; ++j) {
                if (current_slot[j] != 0) {
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }

            if (memcmp(current_slot, element->key, element->keylength) == 0) {
                Element* found = new Element(
                    current_slot,
                    element->keylength,
                    current_slot + handle->MaxKeyLength,
                    handle->MaxPayloadLength
                );

                return found;
            }
        }
        cout << "--Get: Element not found--" << endl;
        return NULL;
    }


    void Print(const Element* element) {
        cout << "Key: " << static_cast<const char*>(element->key) << " Payload: " << static_cast<const char*>(element->payload) << endl;
    }

    BOOL Update(const HTHANDLE* handle, const Element* element, const void* newpayload, int newpayloadlength) {

        if (handle == NULL || handle->Addr == NULL) {
            cout << "--Update: Failed to update an element(handle was invalid)--" << endl;
            return FALSE;
        }

        if (element == NULL || element->keylength == NULL) {
            cout << "--Update: Failed to update an element(element was invalid)--" << endl;
            return FALSE;
        }

        if (newpayload == NULL || newpayloadlength == NULL || newpayloadlength > handle->MaxPayloadLength) {
            cout << "--Update: Failed to update an element(data to update were invalid)--" << endl;
            return FALSE;
        }

        lock_guard<mutex>lock(ht_mutex);

        size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        int metadata_offset = 3 * sizeof(int);
        char* base = static_cast<char*>(handle->Addr) + metadata_offset;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);

            bool is_empty = true;
            for (int j = 0; j < handle->MaxKeyLength; ++j) {
                if (current_slot[j] != 0) {
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }

            if (memcmp(current_slot, element->key, element->keylength) == 0) {
                memcpy(current_slot + handle->MaxKeyLength, newpayload, newpayloadlength);
                cout << "--Update: Element updated--" << endl;
                return TRUE;
            }
        }

        cout << "--Update: Failed to update(element not found)--" << endl;
        return FALSE;


    }
}