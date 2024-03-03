#include "timeutil.h"
#include <sys/time.h>

namespace cutl
{
    uint64_t timestamp()
    {
        struct timeval t;
        gettimeofday(&t, 0);
        return t.tv_sec;
    }

    uint64_t timestamp_ms()
    {
        struct timeval t;
        gettimeofday(&t, 0);
        return (uint64_t)(t.tv_sec * 1000ULL + t.tv_usec / 1000);
    }

    uint64_t timestamp_us()
    {
        struct timeval t;
        gettimeofday(&t, 0);
        return (uint64_t)(t.tv_sec * 1000000ULL + t.tv_usec);
    }

    uint64_t clocktime()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec;
    }

    uint64_t clocktime_ms()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000;
    }

    uint64_t clocktime_us()
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
    }

    uint64_t ms2s(uint64_t time) { return time / 1000; }
    uint64_t us2s(uint64_t time) { return time / 1000000; }
    uint64_t us2ms(uint64_t time) { return time / 1000; }
    uint64_t s2ms(uint64_t time) { return time * 1000; }
    uint64_t s2us(uint64_t time) { return time * 1000000; }
    uint64_t ms2us(uint64_t time) { return time * 1000; }
} // namespace