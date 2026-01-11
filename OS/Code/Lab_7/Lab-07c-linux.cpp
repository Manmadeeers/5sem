#include <stdio.h>
#include <time.h>
#include <signal.h>

volatile int tick = 0;

void handler(int sig) {
    tick++;
}

int main() {
    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigaction(SIGALRM, &sa, NULL);
    timer_t timer;
    struct itimerspec ts;
    timer_create(CLOCK_REALTIME, NULL, &timer);
    ts.it_value.tv_sec = 3;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 3;
    ts.it_interval.tv_nsec = 0;
    timer_settime(timer, 0, &ts, NULL);
    struct timespec start, now;
    clock_gettime(CLOCK_REALTIME, &start);
    unsigned long long counter = 0;
    int ticks = 0;
    while (1) {
        counter++;
        if (tick > 0) {
            tick--;
            ticks++;
            if (ticks < 5) {
                printf("Iterations after %d seconds: %llu\n", ticks * 3, counter);
            } else {
                printf("Final iterations after 15 seconds: %llu\n", counter);
                break;
            }
        }
        clock_gettime(CLOCK_REALTIME, &now);
        if (now.tv_sec - start.tv_sec >= 15)
            break;
    }
    timer_delete(timer);
    return 0;
}
