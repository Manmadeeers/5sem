#define UNICODE
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <tlhelp32.h>

static void WriteConsoleString(LPCWSTR s)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE || hOut == NULL) return;
    DWORD written = 0;
    WriteConsoleW(hOut, s, (DWORD)lstrlenW(s), &written, NULL);
}

static void WriteFmt(LPCWSTR fmt, ...)
{
    WCHAR buf[512];
    va_list ap;
    va_start(ap, fmt);
    wvsprintfW(buf, fmt, ap);
    va_end(ap);
    WriteConsoleString(buf);
    WriteConsoleString(L"\r\n");
}

int wmain(void)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
    {
        WriteConsoleString(L"CreateToolhelp32Snapshot failed\r\n");
        ExitProcess(1);
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(snap, &pe))
    {
        WriteConsoleString(L"Process32FirstW failed\r\n");
        CloseHandle(snap);
        ExitProcess(1);
    }

    WriteFmt(L"%-40s %-10s %-10s", L"ProcessName", L"PID", L"ParentPID");
    do
    {
        WCHAR line[512];
        wsprintfW(line, L"%-40s %-10u %-10u", pe.szExeFile, pe.th32ProcessID, pe.th32ParentProcessID);
        WriteConsoleString(line);
        WriteConsoleString(L"\r\n");
    } while (Process32NextW(snap, &pe));

    CloseHandle(snap);
    ExitProcess(0);
    return 0;
}