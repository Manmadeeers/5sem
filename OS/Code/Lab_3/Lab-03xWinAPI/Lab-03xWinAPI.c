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

static int ParsePositiveInt(LPCWSTR s)
{
    if (s == NULL) return -1;
    while (*s == L' ' || *s == L'\t') s++;
    if (*s == L'\0') return -1;
    long val = 0;
    BOOL any = FALSE;
    while (*s)
    {
        if (*s < L'0' || *s > L'9') return -1;
        any = TRUE;
        int digit = *s - L'0';
        if (val > (LONG_MAX - digit) / 10) return -1;
        val = val * 10 + digit;
        s++;
    }
    if (!any) return -1;
    if (val <= 0) return -1;
    return (int)val;
}

static void IntToWStr(int val, LPWSTR buf, int bufChars)
{
    if (bufChars <= 0) return;
    if (val == 0)
    {
        if (bufChars > 1) { buf[0] = L'0'; buf[1] = L'\0'; }
        else if (bufChars == 1) buf[0] = L'\0';
        return;
    }
    WCHAR tmp[32];
    int pos = 0;
    int v = val;
    while (v > 0 && pos < (int)(sizeof(tmp) / sizeof(tmp[0]) - 1))
    {
        tmp[pos++] = (WCHAR)(L'0' + (v % 10));
        v /= 10;
    }
    int i;
    int outPos = 0;
    for (i = pos - 1; i >= 0 && outPos < bufChars - 1; --i)
        buf[outPos++] = tmp[i];
    buf[outPos] = L'\0';
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd)
{
    SetEnvironmentVariableA("ITER_NUM", "30");
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int iter = -1;
    if (argc >= 2)
    {
        iter = ParsePositiveInt(argv[1]);
    }
    if (iter <= 0)
    {
        DWORD needed = GetEnvironmentVariableW(L"ITER_NUM", NULL, 0);
        if (needed > 0)
        {
            LPWSTR buf = (LPWSTR)LocalAlloc(LMEM_FIXED, needed * sizeof(WCHAR));
            if (buf)
            {
                DWORD r = GetEnvironmentVariableW(L"ITER_NUM", buf, needed);
                if (r > 0 && r < needed)
                {
                    iter = ParsePositiveInt(buf);
                }
                LocalFree(buf);
            }
        }
    }
    if (argv) LocalFree(argv);
    if (iter <= 0)
    {
        WriteConsoleString(L"ERROR: Number of iterations not specified.\r\n");
        ExitProcess(1);
        return 1;
    }
    WriteConsoleString(L"Iterations: ");
    WCHAR numbuf[32];
    IntToWStr(iter, numbuf, sizeof(numbuf) / sizeof(numbuf[0]));
    WriteConsoleString(numbuf);
    WriteConsoleString(L"\r\n");
    DWORD pid = GetCurrentProcessId();
    WCHAR outbuf[128];
    for (int i = 1; i <= iter; ++i)
    {
        WriteConsoleString(L"PID: ");
        IntToWStr((int)pid, numbuf, sizeof(numbuf) / sizeof(numbuf[0]));
        WriteConsoleString(numbuf);
        WriteConsoleString(L", Iteration: ");
        IntToWStr(i, numbuf, sizeof(numbuf) / sizeof(numbuf[0]));
        WriteConsoleString(numbuf);
        WriteConsoleString(L"\r\n");
        Sleep(500);
    }
    ExitProcess(0);
    return 0;
}