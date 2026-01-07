#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct {
    const char* ProcName;
    int IterNum;
} parms;

sem_t* sem;

void FuncForProcess(parms* par)
{
    const char* username = "User-ee4aed31";
    int usernameLength = strlen(username);

    for (int i = 0; i < par->IterNum; i++) {

        if (i == 29) {
            sem_wait(sem); 
        }

        printf("%5s   - %7d     - %5c\n",
               par->ProcName,
               i + 1,
               username[i % usernameLength]);

        usleep(100 * 1000);

        if (i == 59) {
            sem_post(sem);  
        }
    }
}

int main(int argc, char* argv[])
{
    const char* SEM_NAME = "/Lab06Semaphore";

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    if (argc == 2) {
        parms p;
        p.ProcName = argv[1];
        p.IterNum = 90;

        FuncForProcess(&p);

        sem_close(sem);
        return 0;
    }

    // Создаём процессы A и B
    pid_t pidA = fork();
    if (pidA == 0) {
        execlp("./Lab-06b ", " Lab-06b", "A", NULL);
        perror("execlp A");
        exit(1);
    }

    pid_t pidB = fork();
    if (pidB == 0) {
        execlp("./Lab-06b", " Lab-06b", "B", NULL);
        perror("execlp B");
        exit(1);
    }

    parms mainParms;
    mainParms.ProcName = "main";
    mainParms.IterNum = 90;
    FuncForProcess(&mainParms);

    waitpid(pidA, NULL, 0);
    waitpid(pidB, NULL, 0);

    sem_close(sem);
    sem_unlink(SEM_NAME);

    return 0;
}
