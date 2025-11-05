//OS11 HTAPI functions implementation
#include "pch.h"
#include "OS_11DLL.h"
#include <Windows.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
#include <future>

#define METADATA_OFFSET 4*sizeof(int)
//#define DEBUG

using namespace std;


namespace HT {

    struct ScopedNamedMutex {//for multi-process access: locking and releasing mutexes
        HANDLE hMutex;
        bool locked;
        bool ownHandle;

        ScopedNamedMutex(const char*name, DWORD timeout = INFINITE) :hMutex(NULL), locked(false),ownHandle(false) {
            if (name == nullptr) {
                return;
            }
           
            hMutex = CreateMutexA(NULL, FALSE, name);
            if (!hMutex) {
                std::cerr << "--ScopedNamedMutex: CreateMutexA failed. Error: " << GetLastError() << std::endl;
                return;
            }
            ownHandle = true;
    

            DWORD w = WaitForSingleObject(hMutex, timeout);
            if (w == WAIT_OBJECT_0||w==WAIT_ABANDONED) {
                locked = true;
                if (w == WAIT_ABANDONED) {
                    std::cerr << "ScopedNamedMutex: Wait returned WAIT_ABANDONED" << std::endl;
                }
            }
            else {
                std::cerr << "ScopedNamedMutex: WaitForSingleObject returned " << w<<" Error: "<<GetLastError() << std::endl;
            }
        }

        ~ScopedNamedMutex() {
            if (locked && hMutex != NULL) {
                ReleaseMutex(hMutex);
                locked = false;
            }
            if (ownHandle && hMutex != NULL) {
                ownHandle = false;
                CloseHandle(hMutex);
                hMutex = NULL;
            }
        }

        bool isLocked() const { return locked; }
    };

    static size_t safe_strlen(const char* s, size_t maxlen) {
        if (!s) {
            return 0;
        }

        size_t i = 0;

        for (; i < maxlen; ++i) {
            if (s[i] == '/0') {
                return i;
            }
        }
        return maxlen;
    }
    //using classical DJB2 algorithm

    uint hashFunction(const void* key, int keyLength, int capacity) {
        uint hash = 5381;


        const char* str = static_cast<const char*>(key);

        for (int i = 0; i < keyLength; ++i) {


            hash = ((hash << 5) + hash) + str[i];

        }

        std::cout << "--Hash: current Hash value: " << hash << std::endl;


        return hash % capacity;
    }

    static uint mutex_hash(const char* str) {
        uint hash = 5381;

        while (*str) {
            hash = ((hash << 5) + hash) + (unsigned char)(*str++);
        }
        return hash;
    }

    static HANDLE create_open_mutexp(const char* fileName, char* outNameBuf, size_t outNameBufSize) {
        if (!fileName) {
            return NULL;
        }
        uint hash = mutex_hash(fileName);

        char nameBuf[128];

        snprintf(nameBuf, sizeof(nameBuf), "Global\\HT_Mutex_%08X", hash);
        nameBuf[sizeof(nameBuf) - 1] = '\0';

        if (outNameBuf && outNameBufSize > 0) {
            strncpy_s(outNameBuf, outNameBufSize, nameBuf, _TRUNCATE);
        }

        HANDLE hMutex = CreateMutexA(
            NULL,
            FALSE,
            nameBuf
        );

        if (!hMutex) {
            std::cerr << "--create_open_mutexp: CreateMutexA failed. Error: " << GetLastError() << std::endl;
        }

        return hMutex;
    }

    std::mutex ht_mutex;//for multi-thread access

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
        this->CurrentElements = 0;
    }

    HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])

        : Capacity(Capacity), SecSnapshotInterval(SecSnapshotInterval),

        MaxKeyLength(MaxKeyLength), MaxPayloadLength(MaxPayloadLength), lastsnaptime(0),
        File(NULL), FileMapping(NULL), Addr(NULL), CurrentElements(0)
    {

        strcpy_s(this->FileName, 512, FileName);
        this->CurrentElements = 0;

    }



    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {

        lock_guard<mutex>lock(ht_mutex);

        HTHANDLE* ht = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
        std::cout << "----------Creation Started----------" << std::endl << std::endl;

        ht->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,//access mode:read &writes
            0, //sharing between processes: No sharing
            NULL, //security attributes: Default security attributes    
            CREATE_ALWAYS, //how to open: Open if exists, else create
            FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,//file attribute:normal and overlapped for multi-process access
            NULL // No template file
        );

        if (ht->File == INVALID_HANDLE_VALUE) {
            std::cout << "--File Creation Failed(Create)-- Error: " << GetLastError() << std::endl;
            delete ht;
            return NULL;
        }
        else {
            std::cout << "--File Creation Successful(Create)--" << std::endl;
        }

        DWORD fileSize = GetFileSize(ht->File, NULL);
        if (fileSize == INVALID_FILE_SIZE) {
            std::cout << "File error: " << GetLastError() << std::endl;
        }


        int slot_size = (ht->MaxKeyLength + ht->MaxPayloadLength);
        std::cout << "Slot size: " << slot_size << std::endl;

        std::cout << "Metadata offset: " << METADATA_OFFSET << std::endl;

        std::cout << "Storage capacity: " << ht->Capacity << std::endl;
        int storage_size = METADATA_OFFSET + (ht->Capacity * slot_size);
        std::cout << "Storage size: " << storage_size << std::endl;


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
            std::cout << "--File Mapping Failed(Create)-- Error: " << error << std::endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            std::cout << "`--File Mapping Successful(Create)--" << std::endl;
        }


        ht->Addr = MapViewOfFile(
            ht->FileMapping,//handle to file mapping
            FILE_MAP_ALL_ACCESS, //access mode: all access
            0, // Offset high: from the top of the file
            0, // Offset low: from the bottom of the file
            0 // Map the entire file
        );

        if (ht->Addr == NULL) {
            std::cout << "--Failed to Map View Of File(Create)-- Error: " << GetLastError() << std::endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            std::cout << "--MapViewOfFile successful(Create)--" << std::endl;
        }


     

        memcpy(ht->Addr, &Capacity, sizeof(int));
        memcpy(static_cast<char*>(ht->Addr) + sizeof(int), &MaxKeyLength, sizeof(int));
        memcpy(static_cast<char*>(ht->Addr) + 2 * sizeof(int), &MaxPayloadLength, sizeof(int));
        memcpy(static_cast<char*>(ht->Addr) + 3 * sizeof(int), &ht->SecSnapshotInterval, sizeof(int));

        ht->mutex_handle = create_open_mutexp(FileName, ht->mutex_name, sizeof(ht->mutex_name));


        if (ht->mutex_handle == NULL) {
            DWORD err = GetLastError();
            std::cout << "--Create: Failed to create/open global mutex. multi-process access impossible. Error: " << err << std::endl;
            UnmapViewOfFile(ht->Addr);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht);
            delete ht;
            return NULL;
        }

        return ht;
    }


    HTHANDLE* Open(const char FileName[512]) {
        std::cout << "----------Opening Started----------" << std::endl << std::endl;

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
            std::cout << "--File Creation Failed(Open)--" << std::endl;
            std::cout << "Transmitted filename: " << FileName << std::endl;
            delete ht;
            return NULL;
        }
        else {
            std::cout << "--File Creation Successful(Open)--" << std::endl;
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
            std::cout << "--File Mapping Failed(Open)--" << std::endl;
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            std::cout << "--File Mapping Successful(Open)--" << std::endl;
        }

        ht->Addr = MapViewOfFile(
            ht->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );
        if (ht->Addr == NULL) {
            std::cout << "--Map View Of File Failed(Open)--" << std::endl;
            CloseHandle(ht->FileMapping);
            CloseHandle(ht->File);
            delete ht;
            return NULL;
        }
        else {
            std::cout << "--Map View Of File Successful(Open)--" << std::endl;
        }


        memcpy(&ht->Capacity, ht->Addr, sizeof(int));
        memcpy(&ht->MaxKeyLength, static_cast<char*>(ht->Addr) + sizeof(int), sizeof(int));
        memcpy(&ht->MaxPayloadLength, static_cast<char*>(ht->Addr) + 2 * sizeof(int), sizeof(int));
        memcpy(&ht->SecSnapshotInterval, static_cast<char*>(ht->Addr) + 3 * sizeof(int), sizeof(int));


        std::cout << "Current ht->Addr: " << ht->Addr << std::endl;

        int slot_size = ht->MaxKeyLength + ht->MaxPayloadLength;
        std::cout << "Computed slot size: " << slot_size << std::endl;


        char* base = static_cast<char*>(ht->Addr) + METADATA_OFFSET;

        //counting previously inserted elements
        for (int i = 0; i < ht->Capacity; ++i) {

            char* current_slot = base + (i * slot_size);

            bool is_empty = true;

            for (int j = 0; j < ht->MaxKeyLength; ++j) {
                if (current_slot[j] != 0) {
                    is_empty = false;
                    ht->CurrentElements++;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }

        }

        std::cout << "Current elements: " << ht->CurrentElements << std::endl;

        std::cout << "Snapshot interval in seconds: " << ht->SecSnapshotInterval << std::endl;

        std::cout << "Last snap time: " << ht->lastsnaptime << std::endl;

        std::cout << "Computed ht->Addr: " << ht->Addr << std::endl;

        int total_mem = slot_size * ht->Capacity;
        std::cout << "Total mem: " << total_mem << std::endl;


        std::cout << "Opened HT storage has " << ht->Capacity << " capacity, " << ht->MaxKeyLength << " Max key length, " << ht->MaxPayloadLength << " max payload length" << std::endl;


        ht->mutex_handle = create_open_mutexp(FileName, ht->mutex_name, sizeof(ht->mutex_name));

        if (ht->mutex_handle == NULL) {
            DWORD err = GetLastError();

            std::cout << "--Open: Failed to create/open global mutex. Multi-process access impossible. Error: " << err << std::endl;
            UnmapViewOfFile(ht->Addr);
            CloseHandle(ht->FileMapping);
            CloseHandle(ht);
            delete ht;
            return NULL;
        }

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
        std::cout << std::endl << "----------Snap----------" << std::endl;

        if (!hthandle) {
            std::cout << "--Snap: Failed to open the handle-- Error: " << GetLastError() << std::endl;
            return FALSE;
        }

        ScopedNamedMutex cross_proc_lock(hthandle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cout << "--Snap: Failed to acquire cross-process mutex" << std::endl;
            return FALSE;
        }

       

        std::lock_guard<std::mutex>lock(ht_mutex);

        if (hthandle->Addr == NULL) {
            std::cout << "--Snap: Addr was NULL" << std::endl;
            return FALSE;
        }

        //TODO: handle lastsnaptime metadata transmition properly
        hthandle->lastsnaptime = time(nullptr);

        //write down metadata at the beginning of the file
        //memcpy(static_cast<char*>(hthandle->Addr) + 4 * sizeof(int), &hthandle->lastsnaptime, sizeof(time_t));

        const size_t slot_size = (size_t)hthandle->MaxKeyLength + (size_t)hthandle->MaxPayloadLength;
        const size_t data_size = (size_t)METADATA_OFFSET + (size_t)hthandle->Capacity * slot_size;

        if (data_size == 0) {

            std::cout << "--Snap: nothing to write (data_size == 0)" << std::endl;

            return TRUE;
        }

        const char* snapName = CreateSnapshotFileName(hthandle);


        std::cout << "--Snap: snapshot file name: " << snapName << std::endl;

        HANDLE HTSnapshot = CreateFileA(
            snapName,
            GENERIC_WRITE | GENERIC_READ,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (HTSnapshot == INVALID_HANDLE_VALUE) {

            DWORD err = GetLastError();

            std::cout << "--Snap: CreateFileA(snapshot) failed: " << err << std::endl;
            std::cout << std::endl << "----------End----------" << std::endl;

            return FALSE;
        }

        std::cout << "--Snap:Snapshot file created" << std::endl;
        std::cout << "--Snap: data size to write: " << data_size << std::endl;


        if (data_size > 0xFFFFFFFFULL) {

            std::cout << "--Snap: data size too large to write in one call" << std::endl;
            std::cout << std::endl << "----------End----------" << std::endl;

            return FALSE;
        }
        

        DWORD expected = (DWORD)data_size;
        DWORD bytesWritten;

        BOOL writeResult = WriteFile(
            HTSnapshot,
            hthandle->Addr,
            expected,
            &bytesWritten,
            NULL
        );

        std::cout << "Bytes written: " << bytesWritten << std::endl;

        if (!writeResult) {

            DWORD err = GetLastError();

            std::cout << "--Snap: WriteFile Failed. Error: " << err << std::endl;
            std::cout << std::endl << "----------End----------" << std::endl;

            CloseHandle(HTSnapshot);

            return FALSE;
        }
        else if (bytesWritten != expected) {

            DWORD err = GetLastError();

            std::cout << "--Snap: Incomplete write. Error: " << err << std::endl;
            std::cout << std::endl << "----------End----------" << std::endl;

            CloseHandle(HTSnapshot);

            return FALSE;
        }
        else if (!FlushFileBuffers(HTSnapshot)) {

            DWORD err = GetLastError();

            std::cout << "--Snap: FileFlushBuffer() WARNING: " << err << std::endl;
        }
        else {

            std::cout << "--Snap: Snapshot Executed--" << std::endl;
        }

        CloseHandle(HTSnapshot);

        return TRUE;
    }

    BOOL Close(HTHANDLE* hthandle) {

        if (!hthandle) {
            std::cout << "--Close:Failed To Close(Invalid handle)--" << " Error: " << GetLastError() << std::endl;
            return FALSE;
        }

        {
            std::lock_guard<std::mutex> guard(ht_mutex);
        }

        //snapshot execution now asynchronous

        std::future<BOOL> snapshot_result = std::async(Snap, hthandle);

        if (snapshot_result.get()) {
            std::cout << "--Close:Snapshot taken--" << std::endl;
        }
        else {
            std::cout << "--Closing:Failed To Take a Snapshot--" << " Error: " << GetLastError() << std::endl;
            return FALSE;
        }


        {
            std::lock_guard<std::mutex>guard(ht_mutex);

            if (hthandle->Addr != NULL) {
                UnmapViewOfFile(hthandle->Addr);
                std::cout << "--Close: Unmapped View Of File--" << std::endl;
            }
            if (hthandle->FileMapping != NULL) {
                CloseHandle(hthandle->FileMapping);
                std::cout << "--Close: File Mapping Handle Closed--" << std::endl;
            }
            if (hthandle->File != NULL) {
                BOOL result = CloseHandle(hthandle->File);
                if (!result) {
                    std::cout << "--Close:Failed To Close The File Handle--" << GetLastError() << std::endl;
                    return FALSE;
                }
                std::cout << "--Close: File Handle Closed " << std::endl;
            }
            if (hthandle->mutex_handle != NULL) {
                CloseHandle(hthandle->mutex_handle);
                std::cout << "--Close: Global Mutex Handle Closed" << std::endl;
            }
        }

        std::cout << "--Close: Closure Completed" << std::endl;

        delete hthandle;
       

        std::cout << "--Close:Cleanup completed--" << std::endl;

        return TRUE;
    }





    BOOL Insert(HTHANDLE* hthandle,  Element* element) {

        if (hthandle == NULL) {
            std::cout << "--Insert: Failed to insert(Invalid handle)--" << " Error: " << GetLastError() << std::endl;
            return FALSE;
        }
        else if (element == NULL) {
            std::cout << "--Insert: Failed to insert(Invalid element)--" << " Error: " << GetLastError() << std::endl;
            return FALSE;
        }
        else if (hthandle->Addr == NULL) {
            std::cout << "--Insert: Failed to insert(Address was null)" << std::endl;
            return FALSE;
        }

        if (hthandle->CurrentElements >= hthandle->Capacity) {
            std::cout << "--Insert: Failed to insert(attempted to exceed the handle capacity)--" << std::endl;
            return FALSE;
        }

        if (element->keylength > hthandle->MaxKeyLength) {
            std::cout << "--Insert: Failed to insert(Element's key length is too big)--" << std::endl;
            return FALSE;
        }

        if (element->payloadlength > hthandle->MaxPayloadLength) {
            std::cout << "--Insert: Failed to insert(Element's payload length is too big)--" << std::endl;
            return FALSE;
        }
        if (element->keylength == NULL) {
            std::cout << "--Insert: Failed to insert(key length was NULL)" << std::endl;
            return FALSE;
        }
        if (element->payloadlength == NULL) {
            std::cout << "--Insert: Failed to insert(payload length was NULL)" << std::endl;
            return FALSE;
        }

        lock_guard<mutex> lock(ht_mutex);


        ScopedNamedMutex cross_proc_lock(hthandle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "--Insert: failed to acquire cross-process mutex" << std::endl;
            return FALSE;
        }

        const int slot_size = hthandle->MaxKeyLength + hthandle->MaxPayloadLength;
        int hash_index = hashFunction(element->key, element->keylength, hthandle->Capacity);

        std::cout << "--Insert: initial hash index: " << hash_index << std::endl;

        char* base = static_cast<char*>(hthandle->Addr) + METADATA_OFFSET;


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
                std::cout << "--Insert: inserted at index " << hash_index << std::endl;
                return TRUE;
            }

            hash_index = (hash_index + 1) % hthandle->Capacity;
        }

        std::cout << "--Insert: no free slot found--" << std::endl;
        return FALSE;
    }

    
    BOOL Delete(HTHANDLE* handle, const Element* element) {


        std::cout << std::endl << "----------Deletion Started----------" << std::endl << std::endl;
        if (handle == NULL || handle->Addr == NULL) {
            std::cout << "--Delete: Failed to delete an element(handle was invalid)--" << std::endl;
            return FALSE;
        }

        if (element == NULL || element->keylength == NULL) {
            std::cout << "--Delete: Failed to delete an element(element was invalid)--" << std::endl;
            return FALSE;
        }

        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cout << "--Delete: Failed to acquire cross-process mutex" << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex> lock(ht_mutex);

        const size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;

        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

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
            std::cout << "--Delete: Failed to delete an element (element not found)--" << std::endl;
            return FALSE;
        }

        for (int i = index_to_delete + 1; i < handle->CurrentElements; ++i) {
            char* src_location = base + (i * slot_size);
            char* dest_location = base + ((i - 1) * slot_size);
            memcpy(dest_location, src_location, slot_size);
        }


        handle->CurrentElements--;

        std::cout << "Delete: Successfully deleted an element with key: " << static_cast<const char*>(element->key) << std::endl;
        std::cout << "----------Deletion Ended----------" << std::endl;

        return TRUE;
    }

    Element* Get(const HTHANDLE* handle, const Element* element) {

        if (handle == NULL || handle->Addr == NULL) {
            std::cout << "--Get: Failed to get an element(handle was invalid)--" << std::endl;
            return NULL;
        }

        if (element == NULL || element->keylength == NULL) {
            std::cout << "--Get: Failed to get an element(element was invalid)--" << std::endl;
            return NULL;
        }
        lock_guard<mutex>lock(ht_mutex);


        const int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

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

                char* payload_ptr = current_slot + handle->MaxKeyLength;
                //int actual_payload_length = (int)safe_strlen(payload_ptr, handle->MaxPayloadLength);
#ifdef DEBUG
                std::cout << "---GET DEBUG---" << std::endl;
                for (int p = handle->MaxKeyLength; p < handle->MaxKeyLength + handle->MaxPayloadLength; ++p) {
                    std::cout << current_slot[p] << " ";
                }
                std::cout << "---GET DEBUG FINISHED---" << std::endl;

#endif // DEBUG

                Element* found = new Element(
                    current_slot,
                    element->keylength,
                    payload_ptr,
                    handle->MaxPayloadLength
                );

                return found;
            }
        }
        std::cout << "--Get: Element not found--" << std::endl;
        return NULL;
    }


    void Print(const Element* element) {
        if (!element) {
            std::cout << "--Print: Element was NULL" << std::endl;
            return;
        }
        std::cout << "--Print-- Key: " << static_cast<const char*>(element->key) << " Payload: " << static_cast<const char*>(element->payload) << std::endl;
    }

    BOOL Update(const HTHANDLE* handle, const Element* element, const void* newpayload, int newpayloadlength) {

        if (handle == NULL || handle->Addr == NULL) {
            std::cout << "--Update: Failed to update an element(handle was invalid)--" << std::endl;
            return FALSE;
        }

        if (element == NULL || element->keylength == NULL) {
            std::cout << "--Update: Failed to update an element(element was invalid)--" << std::endl;
            return FALSE;
        }

        if (newpayload == NULL || newpayloadlength == NULL || newpayloadlength > handle->MaxPayloadLength) {
            std::cout << "--Update: Failed to update an element(data to update were invalid)--" << std::endl;
            return FALSE;
        }

        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cout << "--Update: failed to acquire cross-process mutex. Error: " << GetLastError() << std::endl;
            return FALSE;
        }

        lock_guard<mutex>lock(ht_mutex);
            
        size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

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
                std::cout << "--Update: Element updated--" << std::endl;
                return TRUE;
            }
        }

        std::cout << "--Update: Failed to update(element not found)--" << std::endl;
        return FALSE;


    }
}