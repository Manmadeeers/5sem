#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string.h>

static void* Lab4x(void* arg) {
    int iterations = *((int*)arg);
    const char* username = "User-ee4aed31";
    const char* username_env = getenv("USER");
    pid_t pid = getpid();
    pid_t tid = syscall(SYS_gettid);
    size_t ulen = username ? strlen(username) : 0;
    if (ulen == 0) ulen = 1;
    for (int i = 1; i <= iterations; ++i) {
        char ch = username[(i - 1) % ulen];
        printf("%d - %d - n.%d - %c\n",
            pid, tid, i, ch);
        usleep(300000);
    }
    return NULL;
}
int main() {
    pthread_t thread1, thread2;
    int iterations1 = 50;
    int iterations2 = 125;
    int main_iterations = 100;
    
    if (pthread_create(&thread1, NULL, Lab4x, &iterations1) != 0) {
        perror("Failed to create thread1");
        return 1;
    }
    
    if (pthread_create(&thread2, NULL, Lab4x, &iterations2) != 0) {
        perror("Failed to create thread2");
        return 1;
    }  
    Lab4x(&main_iterations);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("All threads completed. Main thread exiting.\n");
    return 0;
}
