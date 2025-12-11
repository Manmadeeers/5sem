#include <Windows.h>
#include <stdio.h>

int main(void) {

    DWORD processId = GetCurrentProcessId();
    printf("Process identifier: %lu\n", processId);

    DWORD threadId = GetCurrentThreadId();
    printf("Current thread identifier: %lu\n", threadId);

    DWORD processPriorityClass = GetPriorityClass(GetCurrentProcess());
    printf("Current process priorities class: %lu\n", processPriorityClass);

    int threadPriority = GetThreadPriority(GetCurrentThread());
    printf("Current thread priority: %d\n", threadPriority);

    DWORD_PTR processAffinityMask, systemAffinityMask;
    if (GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask)) {
        printf("Process relation mask: 0x%I64X\n", processAffinityMask);
        printf("System relation mask: 0x%I64X\n", systemAffinityMask);
    }
    else {
        printf("Failed to get process relation mask.\n");
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    printf("Number of processors available for a process: %u\n", sysInfo.dwNumberOfProcessors);

    DWORD processorNumber = GetCurrentProcessorNumber();
    printf("Processor number assigned to a current thread: %lu\n", processorNumber);

    return 0;
}
