#include "pch.h"
#include "Lab_4_HT.h"
#define METADATA_OFFSET 4*sizeof(int)

struct ProcessMutex {
    HANDLE hMutex;
    bool locked;
    bool ownHandle;

    ProcessMutex(const char* name, DWORD timeout = INFINITE) :hMutex(NULL), locked(false), ownHandle(false) {
        if (name == nullptr) {
            return;
        }

        hMutex = CreateMutexA(NULL, FALSE, name);
        if (!hMutex) {
            std::cerr << "ScopedNamedMutex: CreateMutexA() failed for mutex handle. Error code: " << GetLastError() << std::endl;
            return;
        }
        ownHandle = true;


        DWORD w = WaitForSingleObject(hMutex, timeout);
        if (w == WAIT_OBJECT_0 || w == WAIT_ABANDONED) {
            locked = true;
            if (w == WAIT_ABANDONED) {
                std::cerr << "ScopedNamedMutex: Wait returned WAIT_ABANDONED" << std::endl;
            }
        }
        else {
            std::cerr << "ScopedNamedMutex: WaitForSingleObject returned " << w << " Error code: " << GetLastError() << std::endl;
        }
    }
    ~ProcessMutex() {
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

namespace HT {
	std::mutex thread_mutex;

    HTHANDLE::HTHANDLE() :
        Capacity(0),
        SecSnapshotInterval(0),
        MaxKeyLength(0),
        MaxPayloadLength(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        LastSnapTime(0),
        CurrentElements(0)
    {
    }

    HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) :
        Capacity(Capacity),
        SecSnapshotInterval(SecSnapshotInterval),
        MaxKeyLength(MaxKeyLength),
        MaxPayloadLength(MaxPayloadLength),
        LastSnapTime(0),
        File(NULL),
        FileMapping(NULL),
        Addr(NULL),
        CurrentElements(0)
    {
        strcpy_s(this->FileName, 512, FileName);
        this->CurrentElements = 0;
    }

    Element::Element() :
        Key(nullptr),
        KeyLength(0),
        Payload(nullptr),
        PayloadLength(0)
    {

    }

    Element::Element(const void* Key, int KeyLength) :
        Key(Key),
        KeyLength(KeyLength),
        Payload(nullptr),
        PayloadLength(0)
    {

    }

    Element::Element(const void* Key, int KeyLength, const void* Payload, int PayloadLength) :
        Key(Key),
        KeyLength(KeyLength),
        Payload(Payload),
        PayloadLength(PayloadLength)
    {

    }

    Element::Element(Element* OldElement, const void* NewPayload, int NewPayloadLength) :
        Key(OldElement->Key),
        KeyLength(OldElement->KeyLength),
        Payload(NewPayload),
        PayloadLength(NewPayloadLength)
    {

    }

    static void WriteMetadata(HTHANDLE* handle) {
        memcpy(handle->Addr, &handle->Capacity, sizeof(int));
        memcpy(static_cast<char*>(handle->Addr) + sizeof(int), &handle->SecSnapshotInterval, sizeof(int));
        memcpy(static_cast<char*>(handle->Addr) + 2 * sizeof(int), &handle->MaxKeyLength, sizeof(int));
        memcpy(static_cast<char*>(handle->Addr) + 3 * sizeof(int), &handle->MaxPayloadLength, sizeof(int));
        memcpy(static_cast<char*>(handle->Addr) + 3 * sizeof(int), &handle->MaxPayloadLength, sizeof(int));
    }

    static void ReadMetadata(HTHANDLE* handle) {
        memcpy(&handle->Capacity, handle->Addr, sizeof(int));
        memcpy(&handle->SecSnapshotInterval, static_cast<char*>(handle->Addr) + sizeof(int), sizeof(int));
        memcpy(&handle->MaxKeyLength, static_cast<char*>(handle->Addr) + 2 * sizeof(int), sizeof(int));
        memcpy(&handle->MaxPayloadLength, static_cast<char*>(handle->Addr) + 3 * sizeof(int), sizeof(int));
    }

    static const char* CreateSnapshotFileName(HTHANDLE* handle) {
        static char buffer[100];
        char time_buffer[80];
        tm time_info;

        localtime_s(&time_info, &handle->LastSnapTime);
        strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &time_info);
        snprintf(buffer, sizeof(buffer), "Snapshot-%s.htsnap", time_buffer);

        return buffer;
    }

    static unsigned int InsertHashFunction(const void* Key, int KeyLength, int Capacity) {
        int hash = 5381;
        const char* str = static_cast<const char*>(Key);

        for (int i = 0; i < KeyLength; ++i) {


            hash = ((hash << 5) + hash) + str[i];

        }


        return abs(hash % Capacity);
    }

    static unsigned int MutexHashFunction(const char* str) {
        unsigned int hash = 5381;

        while (*str) {
            hash = ((hash << 5) + hash) + (unsigned char)(*str++);
        }
        return hash;
    }

    static HANDLE CreateOpenProcessMutex(const char* fileName, char* outNameBuf, size_t outNameBufSize) {
        if (!fileName) {
            return NULL;
        }
        unsigned int hash = MutexHashFunction(fileName);

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
            std::cerr << "CreateOpenProcessMutex : CreateMutexA() failed for mutex handle. Error code: " << GetLastError() << std::endl;
        }

        return hMutex;
    }


    HTHANDLE* Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]) {
        std::lock_guard<std::mutex>lock(thread_mutex);

        HTHANDLE* handle = new HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);
        
        handle->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (handle->File == INVALID_HANDLE_VALUE) {
            std::cerr << "Create:CreateFileA() for handle->File failed. Error code: " << GetLastError() << std::endl;
            delete handle;
            return NULL;
        }
        DWORD storage_size = GetFileSize(handle->File, NULL);
        if (storage_size == INVALID_FILE_SIZE) {
            std::cerr << "Create: File Size of handle->File was invalid. Error code: " << GetLastError() << std::endl;
            delete handle;
            return NULL;
        }
        
        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        DWORD file_size = METADATA_OFFSET + (handle->Capacity * slot_size);

        handle->FileMapping = CreateFileMappingA(
            handle->File,
            NULL,
            PAGE_READWRITE,
            0,
            file_size,
            "HT_Mapping"
        );
        if (handle->FileMapping == NULL) {
            std::cerr << "Create: CreateFileMappingA() for handle->FileMapping failed. Error code: " << GetLastError() << std::endl;
            CloseHandle(handle->File);
            delete handle;
            return NULL;
        }

        handle->Addr = MapViewOfFile(
            handle->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );
        if (handle->Addr == NULL) {
            std::cerr << "Create: MapViewOfFile() for handle->Addr failed. Error code: " << GetLastError() << std::endl;
            CloseHandle(handle->FileMapping);
            CloseHandle(handle->File);
            delete handle;
            return NULL;
        }


        WriteMetadata(handle);

        handle->MutexHandle = CreateOpenProcessMutex(FileName, handle->MutexName, sizeof(handle->MutexName));
        if (handle->MutexHandle == NULL) {
            std::cerr << "Create: CreateOpenProcessMutex() for handle->MutexHandle failed. Error code: " << GetLastError() << std::endl;
            UnmapViewOfFile(handle->Addr);
            CloseHandle(handle->FileMapping);
            CloseHandle(handle->File);
            delete handle;
            return nullptr;
        }

        return handle;

    }


    HTHANDLE* Open(const char FileName[512]) {
        std::lock_guard<std::mutex>lock(thread_mutex);
        HTHANDLE* handle = new HTHANDLE();

        handle->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (handle->File == INVALID_HANDLE_VALUE) {
            std::cerr << "Open: CreateFileA() for handle->File failed. Error code: " << GetLastError() << std::endl;
            delete handle;
            return NULL;
        }

        handle->FileMapping = CreateFileMappingA(
            handle->File,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            "HT_Mapping"
        );
        if (handle->FileMapping == NULL) {
            std::cerr << "Open: CreateFileMappingA() for handle->FileMapping failed. Error code: " << GetLastError() << std::endl;
            CloseHandle(handle->File);
            delete handle;
            return NULL;
        }

        handle->Addr = MapViewOfFile(
            handle->FileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );
        if (handle->Addr == NULL) {
            std::cerr << "Open: MapViewOfFile() for handle->Addr failed. Error code: " << GetLastError() << std::endl;
            CloseHandle(handle->FileMapping);
            CloseHandle(handle->File);
            delete handle;
            return NULL;
        }

        ReadMetadata(handle);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i*slot_size);
            bool is_empty = true;

            for (int j = 0; j < handle->MaxKeyLength; ++j) {
                if (current_slot[j] != 0) {
                    is_empty = false;
                    handle->CurrentElements++;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }

        }


        handle->MutexHandle = CreateOpenProcessMutex(FileName, handle->MutexName, sizeof(handle->MutexName));
        if (handle->MutexHandle == NULL) {
            std::cerr << "Open: CreateOpenProcessMutex() for handle->MutexHandle failed. Error code: " << GetLastError() << std::endl;
            UnmapViewOfFile(handle->Addr);
            CloseHandle(handle->FileMapping);
            CloseHandle(handle->File);
            delete handle;
            return nullptr;
        }


        return handle;
    }

    BOOL Snap(HTHANDLE* handle) {
        if (!handle) {
            std::cerr << "Snap: Failed to access HTHANDLE instance. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Snap: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        handle->LastSnapTime = time(nullptr);

        HANDLE hSnapshot = CreateFileA(
            CreateSnapshotFileName(handle),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            std::cerr << "Snap: CreateFileA() for hSnapshot failed. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        SIZE_T data_size = handle->CurrentElements * (handle->MaxKeyLength + handle->MaxPayloadLength);
        DWORD data_written;

        BOOL write_result = WriteFile(
            hSnapshot,
            handle->Addr,
            data_size,
            &data_written,
            NULL
        );

        CloseHandle(hSnapshot);

        if (!write_result) {
            std::cerr << "Snap: WriteFile() for hSnapshot failed. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }
        if (data_written != data_size) {
            std::cerr << "Snap: WriteFile() for hSnapshot failed due to bytes loss. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        return TRUE;
    }

    BOOL Close(HTHANDLE* handle) {
        if (!handle) {
            std::cerr << "Close: failed to access HTHANDLE instance. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Close: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        BOOL snapshot_result = HT::Snap(handle);
        //std::future<BOOL> snapshot_result = std::async(Snap, handle);

        if (!snapshot_result) {
            std::cerr << "Close: failed to take a snapshot. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        if (!UnmapViewOfFile(handle->Addr)) {
            std::cerr << "Close: UnmapViewOfFile() for handle->Addr failed. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        if (!CloseHandle(handle->FileMapping)) {
            std::cerr << "Close: CloseHandle() for handle->FileMapping failed. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        if (!CloseHandle(handle->File)) {
            std::cerr << "Close: CloseHandle() for handle->File failed. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        return TRUE;
    }


        
    BOOL Insert(HTHANDLE* handle, Element* element) {
        if (!handle || !element) {
            std::cerr << "Insert: Invalid function parameters" << std::endl;
            return FALSE;
        }

        if (handle->Addr == NULL) {
            std::cerr << "Insert: Failed to access mapped address. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        if (handle->CurrentElements >= handle->Capacity) {
            std::cerr << "Insert: Attempted to exceed storage capacity" << std::endl;
            return FALSE;
        }

        if (element->KeyLength >= handle->MaxKeyLength || element->PayloadLength >= handle->MaxPayloadLength) {
            std::cerr << "Insert: Element instance was invalid. Key or payload length exceed storage parameters" << std::endl;
            return FALSE;
        }

        if (!element->Key || !element->Payload) {
            std::cerr << "Insert: Element instance was invalid. Key or Payload does not exist" << std::endl;
            return FALSE;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Insert: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }


        std::lock_guard<std::mutex> lock(thread_mutex);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        int hash_index = InsertHashFunction(element->Key, element->KeyLength, handle->Capacity);
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
                memcpy(current_slot, element->Key, element->KeyLength);
                if (element->KeyLength < handle->MaxKeyLength) {
                    memset(current_slot + element->KeyLength, 0, handle->MaxKeyLength - element->KeyLength);
                }

                char* payload_area = current_slot + handle->MaxKeyLength;
                memcpy(payload_area, element->Payload, element->PayloadLength);
                if (element->PayloadLength < handle->MaxPayloadLength) {
                    memset(payload_area + element->PayloadLength, 0, handle->MaxPayloadLength - element->PayloadLength);
                }

                handle->CurrentElements++;
                std::cout << "Insert: inserted at index " << hash_index << std::endl;
                return TRUE;
            }
            hash_index = (hash_index + 1) % handle->Capacity;
        }

        std::cerr << "Insert: No free slots available" << std::endl;
        return FALSE;

    }

    BOOL Delete(HTHANDLE* handle, Element* element) {
        if (!handle || !element) {
            std::cerr << "Delete: parameters were invalid" << std::endl;
            return FALSE;
        }

        if (!element->Key || element->KeyLength <= 0||element->KeyLength>=handle->MaxKeyLength) {
            std::cerr << "Delete:element instance was invalid" << std::endl;
            return FALSE;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Delete: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex> lock(thread_mutex);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;
        int index_to_delete = -1;

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

            if (element->KeyLength <= handle->MaxKeyLength && memcmp(current_slot, element->Key, element->KeyLength) == 0) {
                index_to_delete = i;
                break;
            }
        }

        if (index_to_delete == -1) {
            std::cerr << "Delete: Element not found" << std::endl;
            return FALSE;
        }

        for (int i = index_to_delete + 1; i < handle->Capacity; ++i) {
            char* src_location = base + (i * slot_size);
            char* dest_location = base + ((i - 1) * slot_size);
            memcpy(dest_location, src_location, slot_size);
        }

        handle->CurrentElements--;

        std::cout << "Delete: Successfully deleted an element with key: " << static_cast<const char*>(element->Key) << std::endl;

        return TRUE;
    }


    Element* Get(HTHANDLE* handle, Element* element) {
        if (!handle || !element) {
            std::cerr << "Get: function parameters were invalid" << std::endl;
            return nullptr;
        }
        if (!element->Key || element->KeyLength <= 0 || element->KeyLength >= handle->MaxKeyLength) {
            std::cerr << "Get: Element instance was invalid" << std::endl;
            return nullptr;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Get: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return nullptr;
        }

        std::lock_guard<std::mutex>lock(thread_mutex);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
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

            if (memcmp(current_slot, element->Key, element->KeyLength) == 0) {
                Element* found = new Element(current_slot, element->KeyLength, current_slot + handle->MaxKeyLength, handle->MaxPayloadLength);
                return found;
            }
        }

        std::cerr << "Get: element not found" << std::endl;
        return nullptr;
    }

    BOOL Update(HTHANDLE* handle, Element* element, const void* NewPayload, int NewPayloadLength) {
        if (!handle || !element||!NewPayload) {
            std::cerr << "Update: function parameters were invalid" << std::endl;
            return FALSE;
        }
        if (element->KeyLength <= 0 || element->KeyLength >= handle->MaxKeyLength || !element->Payload || element->PayloadLength <= 0 || element->PayloadLength >= handle->MaxPayloadLength) {
            std::cerr << "Update: Element instance was invalid" << std::endl;
            return FALSE;
        }
        if (NewPayloadLength <= 0 || NewPayloadLength >= handle->MaxPayloadLength) {
            std::cerr << "Update: NewPayloadLength was invalid" << std::endl;
            return FALSE;
        }

        ProcessMutex cpMutex(handle->FileName);
        if (!cpMutex.isLocked()) {
            std::cerr << "Update: failed to acquire cross process mutex. Error code: " << GetLastError() << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex>lock(thread_mutex);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
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

            if (memcmp(current_slot, element->Key, element->KeyLength) == 0) {
                memcpy(current_slot + handle->MaxKeyLength, NewPayload, NewPayloadLength);
                std::cout << "Update: Element with key "<<static_cast<const char*>(element->Key)<<" updated" << std::endl;
                return TRUE;
            }
        }

        std::cerr << "Update:  element not found" << std::endl;
        return FALSE;
    }

    void Print(Element* element) {
        std::cout << "Key: " << static_cast<const char*>(element->Key) << " Payload: " << static_cast<const char*>(element->Payload) << std::endl;
    }


}