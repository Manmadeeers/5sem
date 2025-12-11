#include <Windows.h>
#include <stdio.h>

DWORD WINAPI ThreadFunction(LPVOID param) {
    int threadNum = (int)(intptr_t)param;
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();

    for (int i = 1; i <= 1000000; ++i) {
        if (i % 1000 == 0) {
            DWORD processPriorityClass = GetPriorityClass(GetCurrentProcess());
            int threadPriority = GetThreadPriority(GetCurrentThread());
            DWORD processorNumber = GetCurrentProcessorNumber();

            printf("Thread %d, Iteration: %d, PID: %lu, TID: %lu, Process priority class: %lu, Thread priority: %d, Processor: %lu\n",
                threadNum, i, processId, threadId, processPriorityClass, threadPriority, processorNumber);

            Sleep(200);
        }
    }
    return 0;
}

int wmain(int argc, wchar_t* argv[]) {
    

    DWORD_PTR affinityMask = 0xFF;
    DWORD processPriority = NORMAL_PRIORITY_CLASS;
    int threadPriority1 = THREAD_PRIORITY_NORMAL;
    int threadPriority2 = THREAD_PRIORITY_NORMAL;

    if (argc == 5) {
        affinityMask = _wcstoui64(argv[1], NULL, 0);
        processPriority = wcstoul(argv[2], NULL, 0);
        threadPriority1 = _wtoi(argv[3]);
        threadPriority2 = _wtoi(argv[4]);
    }
    else {
        wprintf(L"Arguments were not transmitted. Using default values.\n");
    }

    wprintf(L"Processors mask (P1): 0x%I64X\n", affinityMask);
    wprintf(L"Process priority class (P2): %lu\n", processPriority);
    wprintf(L"First thread priority (P3): %d\n", threadPriority1);
    wprintf(L"Second thread priority (P4): %d\n", threadPriority2);

    SetPriorityClass(GetCurrentProcess(), processPriority);
    SetProcessAffinityMask(GetCurrentProcess(), affinityMask);

    HANDLE hThread1 = CreateThread(NULL, 0, ThreadFunction, (LPVOID)1, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, ThreadFunction, (LPVOID)2, 0, NULL);

    SetThreadPriority(hThread1, threadPriority1);
    SetThreadPriority(hThread2, threadPriority2);

    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    CloseHandle(hThread1);
    CloseHandle(hThread2);

    system("pause");
    return 0;
}
