#include <stdio.h>
#include <time.h>

int main() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    time_t seconds = ts.tv_sec;

    struct tm local = *localtime(&seconds);
    struct tm utc   = *gmtime(&seconds);

    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &local);

    int tz_offset = local.tm_hour - utc.tm_hour;

    if (local.tm_yday != utc.tm_yday) {
        tz_offset += (local.tm_yday > utc.tm_yday) ? 24 : -24;
    }

    printf("%s%+03d\n", buf, tz_offset);
    return 0;
}
