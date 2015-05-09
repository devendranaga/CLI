#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

long sapi_get_time_sec()
{
    return time(0);
}

uint64_t sapi_get_time_msec()
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return ((uint64_t)tv.tv_sec) * 1000 + (tv.tv_usec / 1000);
}

int sapi_is_dst_active()
{
    time_t now;
    struct tm *t;

    now = time(0);
    t = localtime(&now);

    // we are not handling for the DST unavailable flag here
    return t->tm_isdst > 0? 1: 0;
}

#ifdef CONFIG_SYSAPI_TIME_TEST
int main(void)
{
    printf("%d\n", sapi_is_dst_active());

    return 0;
}
#endif

