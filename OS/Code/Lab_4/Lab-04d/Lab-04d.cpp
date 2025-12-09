#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <cstdio>


int main()
{
    DWORD pid = GetCurrentProcessId();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        printf("CreateToolhelp32Snapshot failed: %lu\n", GetLastError());
        return 1;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    if (!Thread32First(hSnap, &te)) {
        printf("Thread32First failed: %lu\n", GetLastError());
        CloseHandle(hSnap);
        return 1;
    }

    printf("Threads for process %lu:\n", (unsigned long)pid);
    do {
        if (te.th32OwnerProcessID == pid) {
            printf(" Thread ID: %lu \n",
                (unsigned long)te.th32ThreadID);
        }
        te.dwSize = sizeof(te);
    } while (Thread32Next(hSnap, &te));

    CloseHandle(hSnap);
    return 0;
}
