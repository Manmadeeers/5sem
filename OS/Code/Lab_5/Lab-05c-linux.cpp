#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <pthread.h>
#include <locale.h>

typedef struct {
    int niceValue;
    const char* name;
} ThreadArgs;

void* threadFunc(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int niceValue = args->niceValue;
    const char* name = args->name;

    pid_t pid = getpid();
    pthread_t ptid = pthread_self();   
    if (setpriority(PRIO_PROCESS, 0, niceValue) == -1) {
        perror("setpriority");
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 1; i <= 1000000; ++i) {
        if (i % 1000 == 0) {
            int currentNice = getpriority(PRIO_PROCESS, 0);
            printf("[%s] Iteration %d, PID=%d, TID=%lu, nice=%d\n",
                   name, i, pid, (unsigned long)ptid, currentNice);
            usleep(200 * 1000);        }
    }

    gettimeofday(&end, NULL);
    long duration = (end.tv_sec - start.tv_sec) * 1000L +
                    (end.tv_usec - start.tv_usec) / 1000L;

    printf("[%s] Work time: %ld ms (PID=%d, pthread_t=%lu)\n",
           name, duration, pid, (unsigned long)ptid);
    return NULL;
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <P1 processors mask> <P2 first thread's nice> <P3 second thread's nice>\n", argv[0]);
        return 1;
    }

    unsigned long long affinityMask = strtoull(argv[1], NULL, 0);
    int nice1 = atoi(argv[2]);
    int nice2 = atoi(argv[3]);

    printf("=========================\n");
    printf("Startup parameters:\n");
    printf("Processors mask (P1): 0x%llx\n", affinityMask);
    printf("First thread's priority (nice) (P2): %d\n", nice1);
    printf("Second thread's priority (nice) (P3): %d\n", nice2);
    printf("=========================\n");

    pthread_t t1, t2;
    ThreadArgs args1 = { nice1, "Thread 1" };
    ThreadArgs args2 = { nice2, "Thread 2" };

    if (pthread_create(&t1, NULL, threadFunc, &args1) != 0) {
        perror("pthread_create t1");
        return 1;
    }
    if (pthread_create(&t2, NULL, threadFunc, &args2) != 0) {
        perror("pthread_create t2");
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("All threads finished.\n");
    return 0;
}
