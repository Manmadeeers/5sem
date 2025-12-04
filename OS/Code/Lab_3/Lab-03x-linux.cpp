#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

static void xwrite(const char *s)
{
    ssize_t len = strlen(s);
    while (len > 0) {
        ssize_t w = write(STDOUT_FILENO, s, len);
        if (w < 0) return;
        s += w;
        len -= w;
    }
}

static int parse_positive_int(const char *s)
{
    if (s == NULL) return -1;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '\0') return -1;
    long val = 0;
    const char *p = s;
    while (*p) {
        if (*p < '0' || *p > '9') return -1;
        int digit = *p - '0';
        if (val > (LONG_MAX - digit) / 10) return -1;
        val = val * 10 + digit;
        p++;
    }
    if (val <= 0) return -1;
    return (int)val;
}

static void int_to_dec(long v, char *buf, size_t bufsize)
{
    if (bufsize == 0) return;
    if (v == 0) {
        if (bufsize > 1) { buf[0] = '0'; buf[1] = '\0'; }
        else buf[0] = '\0';
        return;
    }
    char tmp[32];
    int pos = 0;
    long x = v;
    while (x > 0 && pos < (int)(sizeof(tmp)-1)) {
        tmp[pos++] = '0' + (x % 10);
        x /= 10;
    }
    size_t out = 0;
    for (int i = pos-1; i >= 0 && out + 1 < bufsize; --i) buf[out++] = tmp[i];
    buf[out] = '\0';
}

static void msleep(unsigned int ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {}
}

int main(int argc, char *argv[])
{
    int iterations = -1;

    if (argc >= 2) iterations = parse_positive_int(argv[1]);

    if (iterations <= 0) {
        char *env = getenv("ITER_NUM");
        if (env) iterations = parse_positive_int(env);
    }

    if (iterations <= 0) {
        xwrite("ERROR: Number of iterations not specified.\n");
        _exit(1);
    }

    char buf[128];

    xwrite("Iterations: ");
    int_to_dec(iterations, buf, sizeof(buf));
    xwrite(buf);
    xwrite("\n");

    pid_t pid = getpid();
    char pidbuf[32];
    int_to_dec(pid, pidbuf, sizeof(pidbuf));

    for (int i = 1; i <= iterations; ++i) {
        xwrite("PID: ");
        xwrite(pidbuf);
        xwrite(", Iteration: ");
        int_to_dec(i, buf, sizeof(buf));
        xwrite(buf);
        xwrite("\n");
        msleep(500);
    }

    _exit(0);
}