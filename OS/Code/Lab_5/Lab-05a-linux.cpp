#include <stdio.h>
#include <locale.h>
#include <unistd.h>      
#include <sys/types.h>   
#include <pthread.h>     
#include <sched.h>       /
int main() {

    pid_t processId = getpid();
    printf("Current process identifier: %d\n", processId);
    pthread_t threadId = pthread_self();
    printf("Current thread identifier (pthread_t): %lu\n", (unsigned long)threadId);
    int policy;
    struct sched_param param;
    if (pthread_getschedparam(threadId, &policy, &param) == 0) {
        printf("Planner class (policy): %d\n", policy);
        printf("Current thread priority: %d\n", param.sched_priority);
    } else {
        perror("Failed to get planner parameters");
    }
    long cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of processes available: %ld\n", cpuCount);

    return 0;
}
