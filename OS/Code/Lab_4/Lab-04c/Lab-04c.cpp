#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _WIN32_WINNT 0x0600

#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>

static char* GetLocalUserNameUTF8(void)
{
    DWORD size = 0;
    if (GetUserNameW(NULL, &size) || GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        if (size == 0) size = 256;
    }
    WCHAR* wbuf = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, size * sizeof(WCHAR));
    if (!wbuf) return NULL;
    if (!GetUserNameW(wbuf, &size)) {
        HeapFree(GetProcessHeap(), 0, wbuf);
        return NULL;
    }
    int mbLen = WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, NULL, 0, NULL, NULL);
    if (mbLen <= 0) {
        HeapFree(GetProcessHeap(), 0, wbuf);
        return NULL;
    }
    char* mb = (char*)HeapAlloc(GetProcessHeap(), 0, (size_t)mbLen);
    if (!mb) {
        HeapFree(GetProcessHeap(), 0, wbuf);
        return NULL;
    }
    if (!WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, mb, mbLen, NULL, NULL)) {
        HeapFree(GetProcessHeap(), 0, mb);
        HeapFree(GetProcessHeap(), 0, wbuf);
        return NULL;
    }
    HeapFree(GetProcessHeap(), 0, wbuf);
    return mb;
}

typedef struct ThreadParam {
    int iterations;
} ThreadParam;

static void Lab4x(int iterations) {
    if (iterations <= 0) return;
    char* username = GetLocalUserNameUTF8();
    if (!username || username[0] == '\0') {
        if (username) HeapFree(GetProcessHeap(), 0, username);
        username = (char*)HeapAlloc(GetProcessHeap(), 0, 2);
        if (!username) return;
        username[0] = 'U';
        username[1] = '\0';
    }
    size_t ulen = strlen(username);
    if (ulen == 0) ulen = 1;
    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    for (int i = 1; i <= iterations; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%lu - %lu - th.%d - %c\n", (unsigned long)pid, (unsigned long)tid, i, ch);
        fflush(stdout);
        Sleep(300 + (rand() % 101));
    }
    HeapFree(GetProcessHeap(), 0, username);
}

DWORD WINAPI ThreadProc(LPVOID lpParam) {
    ThreadParam* p = static_cast<ThreadParam*>(lpParam);
    if (!p) return 1;
    Lab4x(p->iterations);
    delete p;
    return 0;
}

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    const char* username = "User-ee4aed31";

    ThreadParam* p1 = new ThreadParam{ 50 };
    ThreadParam* p2 = new ThreadParam{ 125 };

    if (!p1 || !p2) {
        fprintf(stderr, "Allocation failed\n");
        delete p1; delete p2;
        return 1;
    }

    HANDLE h1 = CreateThread(NULL, 0, ThreadProc, p1, 0, NULL);
    if (!h1) {
        fprintf(stderr, "CreateThread h1 failed: %lu\n", GetLastError());
        delete p1; delete p2;
        return 1;
    }

    HANDLE h2 = CreateThread(NULL, 0, ThreadProc, p2, 0, NULL);
    if (!h2) {
        fprintf(stderr, "CreateThread h2 failed: %lu\n", GetLastError());
        CloseHandle(h1);
        delete p2;
        return 1;
    }

    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    size_t ulen = strlen(username);
    if (ulen == 0) ulen = 1;

    for (int i = 1; i <= 100; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%lu - %lu - mn.%d - %c\n", (unsigned long)pid, (unsigned long)tid, i, ch);
        fflush(stdout);

        if (i == 40) {
            if (h2) {
                if (!TerminateThread(h2, 0)) {
                    printf("TerminateThread(h2) failed: %lu\n", GetLastError());
                }
                else {
                    printf("Main: terminated thread2 at %d\n", i);
                }
                CloseHandle(h2);
                h2 = NULL;
            }
        }

        Sleep(300 + (rand() % 101));
    }

    HANDLE handles[2];
    DWORD count = 0;
    if (h1) handles[count++] = h1;
    if (h2) handles[count++] = h2;

    if (count > 0) {
        if (WaitForMultipleObjects(count, handles, TRUE, INFINITE) == WAIT_FAILED) {
            fprintf(stderr, "WaitForMultipleObjects failed: %lu\n", GetLastError());
        }
    }

    if (h1) CloseHandle(h1);
    if (h2) CloseHandle(h2);

    return 0;
}