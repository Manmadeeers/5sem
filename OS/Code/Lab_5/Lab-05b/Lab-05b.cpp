#include <Windows.h>
#include <stdio.h>

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 4) {
        wprintf(L"Usage: Lab-05b <P1 mask> <P2 priority> <P3 priority>\n");
        return 1;
    }

    DWORD_PTR affinityMask = _wcstoui64(argv[1], NULL, 0);
    DWORD priority1 = wcstoul(argv[2], NULL, 0);
    DWORD priority2 = wcstoul(argv[3], NULL, 0);

    wprintf(L"Processors mask (P1): 0x%I64X\n", affinityMask);
    wprintf(L"First process priority class (P2): %lu\n", priority1);
    wprintf(L"Second process priority class (P3): %lu\n", priority2);

    LPCWSTR childPath = L"C:\\Users\\User-ee4aed31\\Desktop\\Lab5\\Lab-05x.exe";

    STARTUPINFOW si1; ZeroMemory(&si1, sizeof(si1)); si1.cb = sizeof(si1);
    PROCESS_INFORMATION pi1; ZeroMemory(&pi1, sizeof(pi1));

    STARTUPINFOW si2; ZeroMemory(&si2, sizeof(si2)); si2.cb = sizeof(si2);
    PROCESS_INFORMATION pi2; ZeroMemory(&pi2, sizeof(pi2));

    if (!CreateProcessW(childPath, NULL, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si1, &pi1)) {
        wprintf(L"Failed to start the first process. Error code: %lu\n", GetLastError());
        return 1;
    }

    if (!CreateProcessW(childPath, NULL, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si2, &pi2)) {
        wprintf(L"Failed to start the second process. Error code: %lu\n", GetLastError());
        // If first succeeded, make sure to clean it up
        TerminateProcess(pi1.hProcess, 1);
        CloseHandle(pi1.hProcess);
        CloseHandle(pi1.hThread);
        return 1;
    }

    if (!SetProcessAffinityMask(pi1.hProcess, affinityMask)) {
        wprintf(L"SetProcessAffinityMask(pi1) failed: %lu\n", GetLastError());
    }
    if (!SetProcessAffinityMask(pi2.hProcess, affinityMask)) {
        wprintf(L"SetProcessAffinityMask(pi2) failed: %lu\n", GetLastError());
    }

    if (!SetPriorityClass(pi1.hProcess, priority1)) {
        wprintf(L"SetPriorityClass(pi1) failed: %lu\n", GetLastError());
    }
    if (!SetPriorityClass(pi2.hProcess, priority2)) {
        wprintf(L"SetPriorityClass(pi2) failed: %lu\n", GetLastError());
    }

    wprintf(L"Child processes started. PID: %lu and %lu\n", pi1.dwProcessId, pi2.dwProcessId);

    WaitForSingleObject(pi1.hProcess, INFINITE);
    WaitForSingleObject(pi2.hProcess, INFINITE);

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}