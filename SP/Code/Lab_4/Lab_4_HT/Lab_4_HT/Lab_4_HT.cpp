#include "pch.h"
#include "Lab_4_HT.h"
#define METADATA_OFFSET 4*sizeof(int)

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

        handle->FileMapping = CreateFileMappingA(
            handle->File,
            NULL,
            PAGE_READWRITE,
            0,
            storage_size,
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


        return handle;
    }

    BOOL Snap(HTHANDLE* handle) {
        if (!handle) {
            std::cerr << "Snap: Failed to access HTHANDLE instance. Error code: " << GetLastError() << std::endl;
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

        std::lock_guard<std::mutex>lock(thread_mutex);
        std::future<BOOL> snapshot_result = std::async(Snap, handle);

        if (!snapshot_result.get()) {
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




}