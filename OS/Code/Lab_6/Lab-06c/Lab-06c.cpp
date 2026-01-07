#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    const char* ProcName;
    int IterNum;
} parms;

HANDLE hSemaphore;

void FuncForProcess(parms* par) {
    const char* username = "User-ee4aed31";
    int usernameLength = strlen(username);

    for (int i = 0; i < par->IterNum; i++) {

        if (i == 29) {
            WaitForSingleObject(hSemaphore, INFINITE);
        }

        printf("%5s   - %7d     - %5c\n",
            par->ProcName,
            i + 1,
            username[i % usernameLength]);

        Sleep(100);

        if (i == 59) {
            ReleaseSemaphore(hSemaphore, 1, NULL);
        }
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    hSemaphore = CreateSemaphoreA(
        NULL,
        1,
        1,
        "Global\\Lab06Semaphore"
    );

    if (hSemaphore == NULL) {
        printf("Ошибка создания семафора\n");
        return 1;
    }

    if (argc == 2) {
        parms p;
        p.ProcName = argv[1];
        p.IterNum = 90;

        FuncForProcess(&p);
        CloseHandle(hSemaphore);
        return 0;
    }

    STARTUPINFOA siA = { sizeof(siA) };
    STARTUPINFOA siB = { sizeof(siB) };
    PROCESS_INFORMATION piA = { 0 };
    PROCESS_INFORMATION piB = { 0 };

    char cmdA[] = "Lab-06c.exe A";
    char cmdB[] = "Lab-06c.exe B";

    CreateProcessA(NULL, cmdA, NULL, NULL, FALSE, 0, NULL, NULL, &siA, &piA);
    CreateProcessA(NULL, cmdB, NULL, NULL, FALSE, 0, NULL, NULL, &siB, &piB);

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
    CloseHandle(hSemaphore);

    return 0;
}
