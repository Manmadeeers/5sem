#define UNICODE
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <shellapi.h>

static void WriteConsoleString(LPCWSTR s)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE || hOut == NULL) return;
    DWORD written = 0;
    WriteConsoleW(hOut, s, (DWORD)lstrlenW(s), &written, NULL);
}

static void WriteFmt(LPCWSTR prefix, LPCWSTR s)
{
    WriteConsoleString(prefix);
    WriteConsoleString(s);
    WriteConsoleString(L"\r\n");
}

static BOOL BuildChildPath(LPWSTR outPath, DWORD outChars)
{
    if (!outPath || outChars == 0) return FALSE;
    DWORD len = GetModuleFileNameW(NULL, outPath, outChars);
    if (len == 0 || len >= outChars) return FALSE;
    WCHAR* lastSlash = NULL;
    for (DWORD i = len; i > 0; --i)
    {
        if (outPath[i - 1] == L'\\' || outPath[i - 1] == L'/')
        {
            lastSlash = outPath + i;
            break;
        }
    }
    if (lastSlash)
    {
        lstrcpyW(lastSlash, L"Lab-03xWinAPI.exe");
        return TRUE;
    }
    return FALSE;
}

int wmain(void)
{
    WCHAR exePath[MAX_PATH];
    if (!BuildChildPath(exePath, MAX_PATH))
    {
        WriteConsoleString(L"ERROR: cannot build child exe path\r\n");
        ExitProcess(1);
    }

    STARTUPINFOW si[3];
    PROCESS_INFORMATION pi[3];
    BOOL created[3] = { FALSE, FALSE, FALSE };
    ZeroMemory(si, sizeof(si));
    for (int i = 0; i < 3; ++i) si[i].cb = sizeof(STARTUPINFOW);
    ZeroMemory(pi, sizeof(pi));

    SetEnvironmentVariableW(L"ITER_NUM", L"10");

    if (CreateProcessW(
        exePath,
        NULL,
        NULL, NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si[0],
        &pi[0]))
    {
        created[0] = TRUE;
        WriteFmt(L"[1] Created (lpApplicationName only): ", exePath);
    }
    else
    {
        WriteConsoleString(L"[1] CreateProcess failed: ");
        DWORD e = GetLastError();
        WCHAR buf[64];
        wsprintfW(buf, L"%u", e);
        WriteFmt(L"", buf);
    }

    WCHAR cmd2[MAX_PATH + 32];
    lstrcpyW(cmd2, exePath);
    lstrcatW(cmd2, L" 30");
    if (CreateProcessW(
        NULL,
        cmd2,
        NULL, NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si[1],
        &pi[1]))
    {
        created[1] = TRUE;
        WriteFmt(L"[2] Created (lpCommandLine only): ", cmd2);
    }
    else
    {
        WriteConsoleString(L"[2] CreateProcess failed: ");
        DWORD e = GetLastError();
        WCHAR buf[64];
        wsprintfW(buf, L"%u", e);
        WriteFmt(L"", buf);
    }

    SetEnvironmentVariableW(L"ITER_NUM", L"20");

    if (CreateProcessW(
        exePath,
        NULL,
        NULL, NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si[2],
        &pi[2]))
    {
        created[2] = TRUE;
        WriteFmt(L"[3] Created (lpApplicationName + local env ITER_NUM=20): ", exePath);
    }
    else
    {
        WriteConsoleString(L"[3] CreateProcess failed: ");
        DWORD e = GetLastError();
        WCHAR buf[64];
        wsprintfW(buf, L"%u", e);
        WriteFmt(L"", buf);
    }

    HANDLE waitHandles[3];
    int waitCount = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (created[i])
        {
            waitHandles[waitCount++] = pi[i].hProcess;
        }
    }

    if (waitCount > 0)
    {
        DWORD waitResult = WaitForMultipleObjects(waitCount, waitHandles, TRUE, INFINITE);
        if (waitResult >= WAIT_OBJECT_0 && waitResult < WAIT_OBJECT_0 + (DWORD)waitCount)
        {
            WriteConsoleString(L"All created child processes have exited.\r\n");
        }
        else
        {
            WriteConsoleString(L"WaitForMultipleObjects failed or returned unexpected value.\r\n");
        }
    }
    else
    {
        WriteConsoleString(L"No child processes were created successfully.\r\n");
    }

    for (int i = 0; i < 3; ++i)
    {
        WCHAR buf[128];
        wsprintfW(buf, L"Child %d: %s", i + 1, created[i] ? L"OK" : L"FAILED");
        WriteFmt(L"", buf);
    }

    for (int i = 0; i < 3; ++i)
    {
        if (created[i])
        {
            DWORD code = 0;
            if (GetExitCodeProcess(pi[i].hProcess, &code))
            {
                WCHAR buf[128];
                wsprintfW(buf, L"Child %d exit code: %u", i + 1, code);
                WriteFmt(L"", buf);
            }
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }
    }

    WriteConsoleString(L"Parent exiting.\r\n");
    ExitProcess(0);
    return 0;
}