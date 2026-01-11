#include <windows.h>
#include <iostream>

bool StartChild(const char* cmdLine, PROCESS_INFORMATION& pi)
{
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    ZeroMemory(&pi, sizeof(pi));

    return CreateProcessA(
        NULL,
        (LPSTR)cmdLine,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    );
}

int main()
{
    PROCESS_INFORMATION pi1, pi2;

    if (!StartChild("cmd /k Lab-07x.exe 60", pi1))
    {
        std::cout << "Failed to start first child process. Error: "
            << GetLastError() << std::endl;
        return 1;
    }

    if (!StartChild("cmd /k Lab-07x.exe 120", pi2))
    {
        std::cout << "Failed to start second child process. Error: "
            << GetLastError() << std::endl;
        return 1;
    }

    HANDLE processes[2] = { pi1.hProcess, pi2.hProcess };

    WaitForMultipleObjects(2, processes, TRUE, INFINITE);

    std::cout << "Both child processes finished.\n";

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}
