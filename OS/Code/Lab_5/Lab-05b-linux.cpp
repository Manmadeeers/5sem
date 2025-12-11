#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sched.h>
#include <locale.h>

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <P1 processors mask> <P2 nice of the first process> <P3 nice of the second process>\n", argv[0]);
        return 1;
    }

    unsigned long long affinityMask = strtoull(argv[1], NULL, 0);
    int nice1 = atoi(argv[2]);
    int nice2 = atoi(argv[3]);

    printf("=========================\n");
    printf("Loading parameters:\n");
    printf("Processors mask (P1): 0x%llx\n", affinityMask);
    printf("First process priority (nice) (P2): %d\n", nice1);
    printf("Second process priority (nice) (P3): %d\n", nice2);
    printf("=========================\n");

    int num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    cpu_set_t set;

    pid_t pid1 = fork();
    if (pid1 == 0) {
        setpriority(PRIO_PROCESS, 0, nice1);

        CPU_ZERO(&set);
        for (int i = 0; i < num_cpus; ++i) {
            if (affinityMask & (1ULL << i)) {
                CPU_SET(i, &set);
            }
        }
        if (sched_setaffinity(0, sizeof(set), &set) == -1) {
            perror("sched_setaffinity");
            _exit(1);
        }

        printf("[Child process 1] PID=%d, nice=%d, processors mask=0x%llx\n",
               getpid(), nice1, affinityMask);

        execlp("xterm", "xterm", "-fa", "Monospace", "-hold", "-e","bash","-c", "./Lab-05x", (char*)NULL);
        perror("Failed to start Lab-05x");
        _exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        
        setpriority(PRIO_PROCESS, 0, nice2);

        CPU_ZERO(&set);
        for (int i = 0; i < num_cpus; ++i) {
            if (affinityMask & (1ULL << i)) {
                CPU_SET(i, &set);
            }
        }
        if (sched_setaffinity(0, sizeof(set), &set) == -1) {
            perror("sched_setaffinity");
            _exit(1);
        }

        printf("[Child process 2] PID=%d, nice=%d, processors mask=0x%llx\n",
               getpid(), nice2, affinityMask);

         execlp("xterm", "xterm", "-fa", "Monospace", "-hold", "-e","bash","-c", "./Lab-05x", (char*)NULL);
        perror("Failed to start Lab-05x");
        _exit(1);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
