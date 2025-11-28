#include "OS13_HTCOM.h"
#define METADATA_OFFSET 4*sizeof(int);

namespace HT_NATIVE {

    //multi thread mutex
    std::mutex thread_mutex;

    //hash function for Inserting elements in HT storage
    uint hashFunction(const void* key, int keyLength, int capacity) {
        uint hash = 5381;

        const char* str = static_cast<const char*>(key);

        for (int i = 0; i < keyLength; ++i) {

            hash = ((hash << 5) + hash) + str[i];
        }

        std::cout << "--Hash: current Hash value: " << hash << std::endl;

        return hash % capacity;
    }

    //hash function for mutexes/events names generating
    static uint mutex_event_hash(const char* str) {
        uint hash = 5381;

        while (*str) {
            hash = ((hash << 5) + hash) + (unsigned char)(*str++);
        }
        return hash;
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



}
