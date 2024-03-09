#include "timeutil.h"
#include <chrono>

namespace cutl
{
    uint64_t timestamp()
    {
        return us2s(timestamp_us());
    }

    uint64_t timestamp_ms()
    {
        return us2ms(timestamp_us());
    }

    // for C++11 and later
    uint64_t timestamp_us()
    {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        return static_cast<uint64_t>(timestamp);
    }

    uint64_t clocktime()
    {
        return us2s(clocktime_us());
    }

    uint64_t clocktime_ms()
    {
        return us2ms(clocktime_us());
    }

    // for C++11 and later
    uint64_t clocktime_us()
    {
        auto run_time = std::chrono::steady_clock::now();
        auto run_time_duration = std::chrono::duration_cast<std::chrono::microseconds>(run_time.time_since_epoch()).count();
        return static_cast<uint64_t>(run_time_duration);
    }

    constexpr static int THOUSAND = 1000;
    constexpr static int MILLION = 1000000;
    uint64_t ms2s(uint64_t time) { return time / THOUSAND; }
    uint64_t us2s(uint64_t time) { return time / MILLION; }
    uint64_t us2ms(uint64_t time) { return time / THOUSAND; }
    uint64_t s2ms(uint64_t time) { return time * THOUSAND; }
    uint64_t s2us(uint64_t time) { return time * MILLION; }
    uint64_t ms2us(uint64_t time) { return time * THOUSAND; }

} // namespace

// // for Unix-like system
// #include <sys/time.h>

// uint64_t timestamp_us()
// {
//     struct timeval t;
//     gettimeofday(&t, 0);
//     return (uint64_t)(t.tv_sec * 1000000ULL + t.tv_usec);
// }

// uint64_t clocktime_us()
// {
//     struct timespec ts;
//     clock_gettime(CLOCK_MONOTONIC, &ts);
//     return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / kThousand;
// }