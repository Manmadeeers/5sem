#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  

typedef struct {
    const char* ThreadName;
    int IterNum;
} parms;

pthread_mutex_t mutex;

void* FuncForThread(void* arg)
{
    parms* par = (parms*)arg;

    const char* username = "User-ee4aed31";
    int usernameLength = strlen(username);

    for (int i = 0; i < par->IterNum; i++) {

        if (i == 29) {
            pthread_mutex_lock(&mutex);
        }

        printf("%5s   - %7d     - %5c\n",
               par->ThreadName,
               i + 1,
               username[i % usernameLength]);

        usleep(100 * 1000); // 100 ms

        if (i == 59) {
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main()
{
    pthread_t threadA, threadB;

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        return 1;
    }

    parms parms1 = { "A", 90 };
    parms parms2 = { "B", 90 };
    parms parms3 = { "main", 90 };

    if (pthread_create(&threadA, NULL, FuncForThread, &parms1) != 0) {
        perror("pthread_create A");
    }

    if (pthread_create(&threadB, NULL, FuncForThread, &parms2) != 0) {
        perror("pthread_create B");
    }

    FuncForThread(&parms3);

    pthread_join(threadA, NULL);
    pthread_join(threadB, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}
