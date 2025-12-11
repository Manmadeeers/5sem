#include <stdio.h>
#include <unistd.h>      
#include <pthread.h>     
#include <sys/time.h>    
#include <time.h>        

void* lab05x_loop(void* arg) {
    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 1; i <= 1000000; ++i) {
        if (i % 1000 == 0) {
            int niceValue = nice(0);

            printf("Iteration: %d, PID: %d, TID: %lu, Patience level: %d\n",
                   i, pid, (unsigned long)tid, niceValue);

            
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 200 * 1000000; 
            nanosleep(&ts, NULL);
        }
    }

    gettimeofday(&end, NULL);

    long duration_ms = (end.tv_sec - start.tv_sec) * 1000L +
                       (end.tv_usec - start.tv_usec) / 1000L;

    printf("Program work time: %ld ms\n", duration_ms);

    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, lab05x_loop, NULL);
    pthread_join(thread, NULL);
    return 0;
}
