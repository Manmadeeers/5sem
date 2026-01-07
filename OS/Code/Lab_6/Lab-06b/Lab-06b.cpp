#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* ProcName;
    int IterNum;
} parms;

HANDLE hMutex;

void FuncForProcess(parms* par) {
    const char* username = "User-ee4aed31";
    int usernameLength = strlen(username);

    for (int i = 0; i < par->IterNum; i++) {

        if (i == 29) {
            WaitForSingleObject(hMutex, INFINITE);
        }

        printf("%5s   - %7d     - %5c\n",
            par->ProcName,
            i + 1,
            username[i % usernameLength]);

        Sleep(100);

        if (i == 59) {
            ReleaseMutex(hMutex);
        }
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    hMutex = CreateMutexA(
        NULL,
        FALSE,
        "Global\\Lab6Mutex"
    );

    if (hMutex == NULL) {
        printf("Ошибка создания mutex\n");
        return 1;
    }

    if (argc == 2) {
        parms p;
        p.ProcName = argv[1];
        p.IterNum = 90;

        FuncForProcess(&p);
        CloseHandle(hMutex);
        return 0;
    }

    STARTUPINFOA siA = { sizeof(siA) };
    STARTUPINFOA siB = { sizeof(siB) };
    PROCESS_INFORMATION piA = { 0 };
    PROCESS_INFORMATION piB = { 0 };

    char cmdA[] = "Lab-06b.exe A";
    char cmdB[] = "Lab-06b.exe B";

    if (!CreateProcessA(
        NULL,
        cmdA,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &siA,
        &piA
    )) {
        printf("Ошибка создания процесса A\n");
    }

    if (!CreateProcessA(
        NULL,
        cmdB,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &siB,
        &piB
    )) {
        printf("Ошибка создания процесса B\n");
    }

    parms mainParms;
    mainParms.ProcName = "main";
    mainParms.IterNum = 90;
    FuncForProcess(&mainParms);

    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piB.hProcess, INFINITE);

    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piB.hProcess);
    CloseHandle(piB.hThread);
    CloseHandle(hMutex);

    return 0;
}
