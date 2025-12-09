#define UNICODE
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    DWORD iterations;
} THREAD_ARG;

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

void Lab4x(int iterations)
{
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

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    THREAD_ARG* p = (THREAD_ARG*)lpParam;
    if (!p) return 1;
    Lab4x((int)p->iterations);
    HeapFree(GetProcessHeap(), 0, p);
    return 0;
}

int main(void)
{
    SetConsoleOutputCP(1251);
    srand((unsigned int)time(NULL));
    const char* fallback_username = "User-ee4aed31";
    THREAD_ARG* p1 = (THREAD_ARG*)HeapAlloc(GetProcessHeap(), 0, sizeof(THREAD_ARG));
    THREAD_ARG* p2 = (THREAD_ARG*)HeapAlloc(GetProcessHeap(), 0, sizeof(THREAD_ARG));
    if (!p1 || !p2) {
        if (p1) HeapFree(GetProcessHeap(), 0, p1);
        if (p2) HeapFree(GetProcessHeap(), 0, p2);
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }
    p1->iterations = 50;
    p2->iterations = 125;
    HANDLE h1 = CreateThread(NULL, 0, ThreadProc, p1, 0, NULL);
    if (!h1) {
        fprintf(stderr, "CreateThread h1 failed: %lu\n", GetLastError());
        HeapFree(GetProcessHeap(), 0, p1);
        HeapFree(GetProcessHeap(), 0, p2);
        return 1;
    }
    HANDLE h2 = CreateThread(NULL, 0, ThreadProc, p2, 0, NULL);
    if (!h2) {
        fprintf(stderr, "CreateThread h2 failed: %lu\n", GetLastError());
        CloseHandle(h1);
        HeapFree(GetProcessHeap(), 0, p2);
        return 1;
    }
    char* username = GetLocalUserNameUTF8();
    if (!username || username[0] == '\0') {
        if (username) HeapFree(GetProcessHeap(), 0, username);
        username = _strdup(fallback_username);
        if (!username) username = _strdup("U");
    }
    size_t ulen = strlen(username);
    if (ulen == 0) ulen = 1;
    DWORD pid = GetCurrentProcessId();
    DWORD tid = GetCurrentThreadId();
    for (int i = 1; i <= 100; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%lu - %lu - mn.%d - %c\n", (unsigned long)pid, (unsigned long)tid, i, ch);
        fflush(stdout);
        if (i == 20) {
            if (h1) {
                DWORD prev = SuspendThread(h1);
                if (prev == (DWORD)-1) printf("SuspendThread(h1) failed: %lu\n", GetLastError());
                else printf("Main: suspended thread1 at %d\n", i);
            }
        }
        if (i == 60) {
            if (h1) {
                DWORD prev = ResumeThread(h1);
                if (prev == (DWORD)-1) printf("ResumeThread(h1) failed: %lu\n", GetLastError());
                else printf("Main: resumed thread1 at %d\n", i);
            }
        }
        if (i == 40) {
            if (h2) {
                DWORD prev = SuspendThread(h2);
                if (prev == (DWORD)-1) printf("SuspendThread(h2) failed: %lu\n", GetLastError());
                else printf("Main: suspended thread2 at %d\n", i);
            }
        }
        Sleep(300 + (rand() % 101));
    }
    if (h2) {
        DWORD prev = ResumeThread(h2);
        if (prev == (DWORD)-1) printf("ResumeThread(h2) failed: %lu\n", GetLastError());
        else printf("Main: resumed thread2 after main loop\n");
    }
    HANDLE arr[2] = { h1, h2 };
    if (WaitForMultipleObjects(2, arr, TRUE, INFINITE) == WAIT_FAILED) {
        fprintf(stderr, "WaitForMultipleObjects failed: %lu\n", GetLastError());
    }
    CloseHandle(h1);
    CloseHandle(h2);
    free(username);
    return 0;
}