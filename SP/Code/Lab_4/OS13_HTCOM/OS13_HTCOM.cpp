#include "OS13_HTCOM.h"
#define METADATA_OFFSET 4*sizeof(int);

namespace HT_NATIVE {

    //multi thread mutex
    std::mutex thread_mutex;

    //Utility hash function for Inserting elements in HT storage
    static uint hashFunction(const void* key, int keyLength, int capacity) {
        uint hash = 5381;

        const char* str = static_cast<const char*>(key);

        for (int i = 0; i < keyLength; ++i) {

            hash = ((hash << 5) + hash) + str[i];
        }

        std::cout << "--Hash: current Hash value: " << hash << std::endl;

        return hash % capacity;
    }

    //Utility hash function for mutexes/events names generating
    static uint mutex_event_hash(const char* str) {
        uint hash = 5381;

        while (*str) {
            hash = ((hash << 5) + hash) + (unsigned char)(*str++);
        }
        return hash;
    }

    //Utility function to create a snapshot file name
    static const char* CreateSnapshotFileName(HTHANDLE* handle) {
        static char buffer[100];
        char time_buffer[80];
        tm time_info;

        localtime_s(&time_info, &handle->LastSnapTime);
        strftime(time_buffer, sizeof(time_buffer), "%Y%m%d_%H%M%S", &time_info);
        snprintf(buffer, sizeof(buffer), "Snapshot-%s.htsnap", time_buffer);

        return buffer;
    }

    //Utility function to write metadata to a head of the file
    void WriteMetadata(HTHANDLE* handle) {

        memcpy(&handle->Capacity, handle->Addr, sizeof(int));
        memcpy(&handle->MaxKeyLength, static_cast<char*>(handle->Addr) + sizeof(int), sizeof(int));
        memcpy(&handle->MaxPayloadLength, static_cast<char*>(handle->Addr) + 2 * sizeof(int), sizeof(int));
        memcpy(&handle->SecSnapshotInterval, static_cast<char*>(handle->Addr) + 3 * sizeof(int), sizeof(int));
    }
    
    //structure to control locking and releasing cross process mutexes in data managing functions(all functions except for Create and Open)
    struct ScopedNamedMutex {
        HANDLE hMutex;
        bool locked;
        bool ownHandle;

        ScopedNamedMutex(const char* name, DWORD timeout = INFINITE) :hMutex(NULL), locked(false), ownHandle(false) {
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
            if (w == WAIT_OBJECT_0 || w == WAIT_ABANDONED) {
                locked = true;
                if (w == WAIT_ABANDONED) {
                    std::cerr << "ScopedNamedMutex: Wait returned WAIT_ABANDONED" << std::endl;
                }
            }
            else {
                std::cerr << "ScopedNamedMutex: WaitForSingleObject returned " << w << " Error: " << GetLastError() << std::endl;
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

    //function to lock/release cross process mutexes in Create and Open functions
    static HANDLE create_open_mutexp(const char* fileName, char* outNameBuf, size_t outNameBufSize) {
        if (!fileName) {
            return NULL;
        }
        uint hash = mutex_event_hash(fileName);

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

    //Native function to insert an ELEMENT into an HT storage
    BOOL InsertNative(HTHANDLE* handle, ELEMENT* element) {
        if (!handle || !element) {
            std::cerr << "Insert:invalid parameters" << std::endl;
            return FALSE;
        }
        if (element->KeyLength <= 0 || element->KeyLength > handle->MaxKeyLength) {
            std::cerr << "Insert:element key length was invalid" << std::endl;
            return FALSE;
        }
        if (element->PayloadLength <= 0 || element->PayloadLength > handle->MaxPayloadLength) {
            std::cerr << "Insert:element payload length was invalid" << std::endl;
            return FALSE;
        }
        if (handle->CurrentElements >= handle->Capacity) {
            std::cerr << "Insert: storage is full. Insertion unavailable" << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex>lock(thread_mutex);

        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "Insert: failed to acquire cross process mutex" << std::endl;
            return FALSE;
        }

        const int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        int hash_index = hashFunction(element->Key, element->KeyLength, handle->Capacity);
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

        for (int probe = 0; probe < handle->Capacity; ++probe) {
            char* slot_key = base + (hash_index * slot_size);

            bool is_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; ++k) {
                if (slot_key[k] != 0) {
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                memcpy(slot_key, element->Key, element->KeyLength);
                if (element->KeyLength < handle->MaxKeyLength) {
                    memset(slot_key + element->KeyLength, 0, handle->MaxKeyLength - element->KeyLength);
                }

                char* payload_area = slot_key + handle->MaxKeyLength;
                memcpy(payload_area, element->Payload, element->PayloadLength);
                if (element->PayloadLength < handle->MaxPayloadLength) {
                    memset(payload_area + element->PayloadLength, 0, handle->MaxPayloadLength - element->PayloadLength);
                }

                handle->CurrentElements++;
                std::cout << "Insert: Element inserted at index " << hash_index << std::endl;
                return TRUE;
            }

            hash_index = (hash_index + 1) % handle->Capacity;
        }
        std::cout << "Insert: No free slots found.Unable to insert" << std::endl;
        return FALSE;

    }   

    //Native function to delete an ELEMENT form an HT storage
    BOOL DeleteNative(HTHANDLE* handle, ELEMENT* element) {
        if (!handle || !element) {
            std::cerr << "Delete:parameters were invalid" << std::endl;
            return FALSE;
        }
        if (element->KeyLength <= 0 || element->KeyLength >= handle->MaxKeyLength) {
            std::cerr << "Delete: element key length was invalid" << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex>lock(thread_mutex);

        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "Delete: failed to acquire cross process mutex" << std::endl;
            return FALSE;
        }

        const size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;
        int index_to_delete = -1;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);

            bool is_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; ++k) {
                if (current_slot[k] != 0) {
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
            std::cerr << "Delete:Element not found. Unable to delete" << std::endl;
            return FALSE;
        }

        for (int i = index_to_delete + 1; i < handle->CurrentElements; ++i) {
            char* src_location = base + (i * slot_size);
            char* dest_location = base + ((i - 1) * slot_size);
            memcpy(dest_location, src_location, slot_size);
        }
        handle->CurrentElements--;

        std::cout << "Delete: Element with key " << static_cast<const char*>(element->Key) << " deleted" << std::endl;
        return FALSE;

    }

    //Native function to get an ELEMENT from an HT storage
    ELEMENT* GetNative(HTHANDLE* handle, ELEMENT* element) {
        if (!handle || !element) {
            std::cerr << "Get: parameters were invalid" << std::endl;
            return FALSE;
        }
        if (element->KeyLength <= 0 || element->KeyLength >= handle->MaxKeyLength) {
            std::cerr << "Get: element key length was invalid" << std::endl;
            return FALSE;
        }
        
        std::lock_guard<std::mutex>lock(thread_mutex);
        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "Get: failed to aquire cross process mutex" << std::endl;
            return FALSE;
        }

        const size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);

            bool is_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; +k) {
                if (current_slot[k] != 0) {
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }
            
            if (memcmp(current_slot, element->Key, element->KeyLength) == 0) {
                char* payload_ptr = current_slot + handle->MaxKeyLength;

                ELEMENT* found_element = new ELEMENT;
                found_element->Key = current_slot;
                found_element->KeyLength = element->KeyLength;
                found_element->Payload = payload_ptr;
                found_element->PayloadLength = handle->MaxPayloadLength;

                return found_element;
            }
        }
        std::cout << "Get: Element with key " << static_cast<const char*>(element->Key) << " was not found" << std::endl;
        return NULL;
    }

    //Native function to update an ELEMENT present in an HT storage
    BOOL UpdateNative(HTHANDLE* handle, ELEMENT* oldElement, const void* newPayload, int newPayloadLength) {
        if (!handle || !oldElement) {
            std::cerr << "Update: parameters were invalid" << std::endl;
            return FALSE;
        }
        if (oldElement->KeyLength <= 0 || oldElement->KeyLength >= handle->MaxKeyLength) {
            std::cerr << "Update: old element's key length was invalid" << std::endl;
            return FALSE;
        }
        if (!newPayload || newPayloadLength <= 0 || newPayloadLength >= handle->MaxPayloadLength) {
            std::cerr << "Update: new payload was invalid" << std::endl;
            return FALSE;
        }

        std::lock_guard<std::mutex>lock(thread_mutex);
        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "Update: failed to acquire cross process mutex" << std::endl;
            return FALSE;
        }

        size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);

            bool is_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; ++k) {
                if (current_slot[k] != 0) {
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                continue;
            }

            if (memcmp(current_slot, oldElement->Key, oldElement->KeyLength) == 0) {
                memcpy(current_slot + handle->MaxKeyLength, newPayload, newPayloadLength);

                std::cout << "Update: element with key "<<static_cast<const char*>(oldElement->Key)<<" was updated" << std::endl;
                return TRUE;
            }
        }

        std::cout << "Update: element with key " << static_cast<const char*>(oldElement->Key) << " was not updated" << std::endl;
        return FALSE;
    }

    //Native function to execute a snapshot of an HT storage
    BOOL SnapNative(HTHANDLE* handle) {
        if (!handle) {
            std::cerr << "Snap: parameters were invalid" << std::endl;
            return FALSE;
        }
        std::lock_guard<std::mutex>lock(thread_mutex);
        ScopedNamedMutex cross_proc_lock(handle->FileName);
        if (!cross_proc_lock.isLocked()) {
            std::cerr << "Snap: failed to acquire cross process mutex" << std::endl;
            return FALSE;
        }

        handle->LastSnapTime = time(nullptr);

        const size_t slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        const size_t data_size = (size_t)METADATA_OFFSET + (size_t)handle->Capacity * slot_size;

        if (data_size == 0) {
            std::cout << "Snap:Nothing to write. Data size is null" << std::endl;
            return TRUE;
        }
        if (data_size > 0xFFFFFFFFULL) {
            std::cerr << "Snap: data size was topo large to be written in one call" << std::endl;
            return FALSE;
        }

        const char* snapshot_filename = CreateSnapshotFileName(handle);

        HANDLE hSnapshot = CreateFileA(
            snapshot_filename,
            GENERIC_WRITE | GENERIC_READ,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            DWORD err = GetLastError();
            std::cerr << "Snap: filed to create a snapshot file. Error code: " << err << std::endl;
            return FALSE;
        }

        DWORD expected = (DWORD)data_size;
        DWORD written;

        BOOL write_result = WriteFile(
            hSnapshot,
            handle->Addr,
            expected,
            &written,
            NULL
        );
        if (!write_result) {
            DWORD err = GetLastError();
            std::cerr << "Snap: failed to write to a snapshot file. Error code: " << err << std::endl;
            CloseHandle(hSnapshot);
            return FALSE;   
        }
        if (written != expected) {
            DWORD err = GetLastError();
            std::cerr << "Snap: incomplete WriteFile operation. Error code: " << err << std::endl;
            return FALSE;
        }
        if (!FlushFileBuffers(hSnapshot)) {
            DWORD err = GetLastError();
            std::cerr << "Snap: FlushFileBuffers() failed. Error code: " << err << std::endl;
            return FALSE;
        }

        std::cout << "Snap: snapshot executed. Snapshot file name: " << snapshot_filename << std::endl;
        CloseHandle(hSnapshot);
        return TRUE;
    }

    //Native function to close an HT storage
    BOOL CloseNative(HTHANDLE* handle) {
        if (!handle) {
            std::cerr << "Close: parameters were invalid" << std::endl;
            return FALSE;
        }
        {
            std::lock_guard<std::mutex>lock(thread_mutex);
        }

        std::future<BOOL> snapshot_result = std::async(SnapNative, handle);

        if (snapshot_result.get()) {
            std::cout << "--Close:Snapshot taken--" << std::endl;
        }
        else {
            std::cout << "--Closing:Failed To Take a Snapshot--" << " Error: " << GetLastError() << std::endl;
            return FALSE;
        }
        {
            std::lock_guard<std::mutex>lock(thread_mutex);
            ScopedNamedMutex cross_proc_lock(handle->FileName);
            if (!cross_proc_lock.isLocked()) {
                std::cerr << "Close: failed to acquire cross process mutex" << std::endl;
                return FALSE;
            }

            if (handle->Addr == NULL || handle->FIleMapping == NULL || handle->File == NULL) {
                std::cerr << "Close: memory corrupted. Unable to close" << std::endl;
                return FALSE;
            }

            if (!UnmapViewOfFile(handle->Addr)) {
                std::cerr << "Close: UnmapViewOfFile failed. Error code: " << GetLastError() << std::endl;
                return FALSE;
            }
            if (!CloseHandle(handle->FIleMapping)) {
                std::cerr << "Close: CloseHandle() for FileMapping failed. Error code: " << GetLastError() << std::endl;
                return FALSE;
            }
            if (!CloseHandle(handle->File)) {
                std::cerr << "Close: CloseHandle() for File failed. Error code: " << GetLastError() << std::endl;
                return FALSE;
            }
            if (!CloseHandle(handle->MutexHandle)) {
                std::cerr << "Close: CloseHandle() for MutexHandle failed. Error code: " << GetLastError() << std::endl;
                return FALSE;
            }
            delete handle;
            std::cout << "Close: closure and cleanup completed" << std::endl;
            return TRUE;
        }
    }

    

}


class OS13 :public IHTStorage {
private:
    ULONG volatile m_ref;
public:
    OS13() :m_ref(1){}
    virtual ~OS13(){}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IHTStorage) {
            *ppv = static_cast<IHTStorage*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return InterlockedIncrement(&m_ref);
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ref_count = _InterlockedDecrement(&m_ref);
        if (ref_count == 0) {
            delete this;
        }
        return ref_count;
    }

    HRESULT STDMETHODCALLTYPE WriteHeader(HTHANDLE* pHandle) override{
        if (!pHandle) {
            return E_INVALIDARG;
        }
        
        HT_NATIVE::WriteMetadata(pHandle);

        return S_OK;
        
    }

    HRESULT STDMETHODCALLTYPE Create(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char* FileName, HTHANDLE** ppHandle) override{
        if (!FileName || !ppHandle) {
            return E_INVALIDARG;
        }

        HTHANDLE* handle = new HTHANDLE();
        handle->Capacity = Capacity;
        handle->SecSnapshotInterval = SecSnapshotInterval;
        handle->MaxKeyLength = MaxKeyLength;
        handle->MaxPayloadLength = MaxPayloadLength;
        handle->CurrentElements = 0;
        strcpy_s(handle->FileName, FileName);
        handle->LastSnapTime = time(nullptr);

        handle->File = CreateFileA(
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            NULL
        );

        if (handle->File == INVALID_HANDLE_VALUE) {
            std::cerr << "Creat: CreateFileA() for handle->File failed" << std::endl;
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        handle->FIleMapping = CreateFileMappingA(
            handle->File,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            "SharedHTMapping"
        );

        if (handle->FIleMapping == NULL) {
            std::cerr << "Create: CreateFileMappingA() for handle.File failed" << std::endl;
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        handle->Addr = MapViewOfFile(
            handle->FIleMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );

        if (handle->Addr == NULL) {
            std::cerr << "Create: MapViewOfFile() for handle.FileMapping failed" << std::endl;
            CloseHandle(handle->FIleMapping);
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        //writing metadata
        WriteHeader(handle);

        handle->MutexHandle = HT_NATIVE::create_open_mutexp(handle->FileName, handle->MutexName, sizeof(handle->MutexName));
        if (handle->MutexHandle == NULL) {
            std::cerr << "Create: create_open_mutexp() for handle->MutexHandle failed" << std::endl;
            UnmapViewOfFile(handle->Addr);
            CloseHandle(handle->FIleMapping);
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        *ppHandle = handle;
        return S_OK;

    }

    HRESULT STDMETHODCALLTYPE Open(const char* FileName, HTHANDLE** ppHandle) override{
        if (!FileName || !ppHandle) {
            return E_INVALIDARG;
        }

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
            std::cerr << "Open: CreateFileA() for handle->File failed" << std::endl;
            delete handle;
            return E_FAIL;
        }

        handle->FIleMapping = CreateFileMappingA(
            handle->File,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            "SharedHTMapping"
        );

        if (handle->FIleMapping == NULL) {
            std::cerr << "Open: CreateFileNappingA() for handle->FileMapping failed" << std::endl;
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        handle->Addr = MapViewOfFile(
            handle->FIleMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            0
        );

        if (handle->Addr == NULL) {
            std::cerr << "Open: MapViewOfFile() for handle->Addr failed" << std::endl;
            CloseHandle(handle->FIleMapping);
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        WriteHeader(handle);

        int slot_size = handle->MaxKeyLength + handle->MaxPayloadLength;
        char* base = static_cast<char*>(handle->Addr) + METADATA_OFFSET;

        for (int i = 0; i < handle->Capacity; ++i) {
            char* current_slot = base + (i * slot_size);
            
            bool is_empty = true;
            for (int k = 0; k < handle->MaxKeyLength; ++k) {
                if (current_slot[k] != 0) {
                    handle->CurrentElements++;
                    is_empty = false;
                    break;
                }
            }

            if (is_empty) {
                continue;

            }
        }

        handle->MutexHandle = HT_NATIVE::create_open_mutexp(FileName);
        if (handle->MutexHandle == NULL) {
            std::cerr << "Open: create_open_mutexp() for handle->MutexHandle failed" << std::endl;
            UnmapViewOfFile(handle->Addr);
            CloseHandle(handle->FIleMapping);
            CloseHandle(handle->File);
            delete handle;
            return E_FAIL;
        }

        *ppHandle = handle;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Close(HTHANDLE* pHandle) override{
        if (!pHandle) {
            return E_INVALIDARG;
        }
        BOOL result = HT_NATIVE::CloseNative(pHandle);
        delete pHandle;

        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Insert(HTHANDLE* pHandle,  ELEMENT* element) override{
        if (!pHandle || !element) {
            return E_INVALIDARG;
        }
        HRESULT result = HT_NATIVE::InsertNative(pHandle, element);

        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Delete(HTHANDLE* pHandle, ELEMENT* element) override{
        if (!pHandle || !element) {
            return E_INVALIDARG;
        }
        HRESULT result = HT_NATIVE::DeleteNative(pHandle, element);

        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Update(HTHANDLE* pHandle, ELEMENT* oldElement, const void* NewPayload, int NewPayloadLength) override {
        if (!pHandle || !oldElement) {
            return E_INVALIDARG;
        }

        HRESULT result = HT_NATIVE::UpdateNative(pHandle, oldElement, NewPayload, NewPayloadLength);

        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Get(HTHANDLE* pHandle, ELEMENT* Element,/*out*/ ELEMENT** ppElement) override{
        if (!pHandle || !Element) {
            return E_INVALIDARG;
        }

        *ppElement = HT_NATIVE::GetNative(pHandle, Element);

        return *ppElement ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Snap(HTHANDLE* handle) override{
        if (!handle) {
            return E_INVALIDARG;
        }

        HRESULT result = HT_NATIVE::SnapNative(handle);

        return result ? S_OK : E_FAIL;
    }

    HRESULT STDMETHODCALLTYPE Print(ELEMENT* element) override {
        if (!element) {
            return E_INVALIDARG;
        }

        std::cout << "Element info: KEY: " << static_cast<const char*>(element->Key) << " PAYLOAD: " << static_cast<const char*>(element->Payload) << std::endl;
        return S_OK;
    }
};
