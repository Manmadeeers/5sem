#include <Windows.h>
#include <stdio.h>

int main(void) {
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();

    DWORD startTime = GetTickCount();

    for (int i = 1; i <= 1000000; ++i) {
        if (i % 1000 == 0) {
            DWORD processPriorityClass = GetPriorityClass(GetCurrentProcess());
            int threadPriority = GetThreadPriority(GetCurrentThread());
            DWORD processorNumber = GetCurrentProcessorNumber();

            printf("Iteration: %d, PID: %lu, TID: %lu, Process priority class: %lu, Thread priority: %d, Processor: %lu\n",
                i, processId, threadId, processPriorityClass, threadPriority, processorNumber);

            Sleep(200);
        }
    }

    DWORD endTime = GetTickCount();
    DWORD duration = endTime - startTime;
    printf("Work time: %lu ms\n", duration);

    system("pause");
    return 0;
}
