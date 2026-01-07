#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* ProcName;
    int StartIter;
    int IterNum;
} parms;

HANDLE hStartEvent;

void FuncForProcess(parms* par) {
    const char* username = "User-ee4aed31";
    int len = strlen(username);

    for (int i = 0; i <= par->IterNum; i++) {
        printf("%5s   - %7d     - %5c\n",
            par->ProcName,
            i,
            username[(i - 1) % len]);

        Sleep(100);
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    hStartEvent = CreateEventA(
        NULL,
        TRUE,
        FALSE,
        "Global\\StartEvent"
    );

    if (hStartEvent == NULL) {
        printf("Ошибка создания события\n");
        return 1;
    }
    if (argc == 2) {
        WaitForSingleObject(hStartEvent, INFINITE);

        parms p;
        p.ProcName = argv[1];
        p.IterNum = 90;

        FuncForProcess(&p);
        CloseHandle(hStartEvent);
        return 0;
    }

    STARTUPINFOA siA = { sizeof(siA) };
    STARTUPINFOA siB = { sizeof(siB) };
    PROCESS_INFORMATION piA = { 0 };
    PROCESS_INFORMATION piB = { 0 };
    char cmdA[] = "Lab-06d.exe A";
    char cmdB[] = "Lab-06d.exe B";

    CreateProcessA(NULL, cmdA, NULL, NULL, FALSE, 0, NULL, NULL, &siA, &piA);
    CreateProcessA(NULL, cmdB, NULL, NULL, FALSE, 0, NULL, NULL, &siB, &piB);

    const char* username = "User-512b44d0";
    int len = strlen(username);

    for (int i = 0; i <= 90; i++) {
        printf("%5s   - %7d     - %5c\n",
            "main",
            i,
            username[(i - 1) % len]);

        Sleep(100);
        if (i == 15) {
            SetEvent(hStartEvent);
        }
    }

    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piB.hProcess, INFINITE);

    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piB.hProcess);
    CloseHandle(piB.hThread);
    CloseHandle(hStartEvent);

    return 0;
}
