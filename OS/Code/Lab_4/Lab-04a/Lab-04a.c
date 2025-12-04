#define UNICODE
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



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


DWORD WINAPI Lab_04x(LPVOID lpParam)
{

    if (!lpParam) {

        printf("Lab_04x: invalid parameter\n");

        return 1;

    }

    THREAD_ARG* arg = (THREAD_ARG*)lpParam;

    DWORD iterations = arg->iterations;

    if (iterations == 0) {

        printf("Lab_04x: iterations == 0\n");

        return 1;

    }

    char* username = GetLocalUserNameUTF8();

    if (!username || username[0] == '\0') {

        if (username) HeapFree(GetProcessHeap(), 0, username);

        username = (char*)HeapAlloc(GetProcessHeap(), 0, 2);

        if (!username) {

            printf("Lab_04x: cannot allocate memory for username\n");

            return 1;

        }

        username[0] = 'U';

        username[1] = '\0';

    }

    size_t uname_len = strlen(username);

    if (uname_len == 0) uname_len = 1;

    DWORD pid = GetCurrentProcessId();

    DWORD tid = GetCurrentThreadId();

    for (DWORD i = 1; i <= iterations; ++i) {

        size_t idx = (i - 1) % uname_len;
        printf("%u \u2013 %u - \u2116%u \u2013 %c\n", (unsigned)pid, (unsigned)tid, (unsigned)i, username[idx]);

        fflush(stdout);

        Sleep(350); 

    }

    HeapFree(GetProcessHeap(), 0, username);

    return 0;

}


int main(void)
{
    SetConsoleOutputCP(1251);

    THREAD_ARG* arg1 = (THREAD_ARG*)HeapAlloc(GetProcessHeap(), 0, sizeof(THREAD_ARG));

    THREAD_ARG* arg2 = (THREAD_ARG*)HeapAlloc(GetProcessHeap(), 0, sizeof(THREAD_ARG));

    if (!arg1 || !arg2) {

        fprintf(stderr, "main: HeapAlloc failed\n");

        if (arg1) HeapFree(GetProcessHeap(), 0, arg1);

        if (arg2) HeapFree(GetProcessHeap(), 0, arg2);

        return 1;

    }

    arg1->iterations = 50;  

    arg2->iterations = 125;  

    HANDLE hThread1 = CreateThread(NULL, 0, Lab_04x, arg1, 0, NULL);

    if (!hThread1) {

        fprintf(stderr, "CreateThread(1) failed: %lu\n", GetLastError());

        HeapFree(GetProcessHeap(), 0, arg1);

        HeapFree(GetProcessHeap(), 0, arg2);

        return 1;

    }

    HANDLE hThread2 = CreateThread(NULL, 0, Lab_04x, arg2, 0, NULL);

    if (!hThread2) {

        fprintf(stderr, "CreateThread(2) failed: %lu\n", GetLastError());

        WaitForSingleObject(hThread1, INFINITE);

        CloseHandle(hThread1);

        HeapFree(GetProcessHeap(), 0, arg1);

        HeapFree(GetProcessHeap(), 0, arg2);

        return 1;

    }

    THREAD_ARG mainArg;

    mainArg.iterations = 100;


    Lab_04x(&mainArg);
    HANDLE waits[2] = { hThread1, hThread2 };

    DWORD waitRes = WaitForMultipleObjects(2, waits, TRUE, INFINITE);

    if (waitRes == WAIT_FAILED) {

        fprintf(stderr, "WaitForMultipleObjects failed: %lu\n", GetLastError());

    }

    DWORD exit1 = 0, exit2 = 0;

    if (!GetExitCodeThread(hThread1, &exit1)) exit1 = (DWORD)-1;

    if (!GetExitCodeThread(hThread2, &exit2)) exit2 = (DWORD)-1;

    printf("Thread1 exit code: %lu\n", exit1);

    printf("Thread2 exit code: %lu\n", exit2);

    CloseHandle(hThread1);

    CloseHandle(hThread2);

    HeapFree(GetProcessHeap(), 0, arg1);

    HeapFree(GetProcessHeap(), 0, arg2);

    return 0;

}