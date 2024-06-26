/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file timeutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

#include "timeutil.h"
#include <chrono>

namespace cutl
{
    uint64_t timestamp(timeunit unit)
    {
        // for C++11 and later
        auto now = std::chrono::system_clock::now();
        auto timestamp_ms = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        auto us = static_cast<uint64_t>(timestamp_ms);
        uint64_t t = 0;
        switch (unit)
        {
        case timeunit::s:
            t = us2s(us);
            break;
        case timeunit::ms:
            t = us2ms(us);
            break;
        case timeunit::us:
            t = us;
            break;
        default:
            break;
        }
        return t;
    }

    uint64_t clocktime(timeunit unit)
    {
        // for C++11 and later
        auto run_time = std::chrono::steady_clock::now();
        auto run_time_duration = std::chrono::duration_cast<std::chrono::microseconds>(run_time.time_since_epoch()).count();
        auto us = static_cast<uint64_t>(run_time_duration);

        uint64_t t = 0;
        switch (unit)
        {
        case timeunit::s:
            t = us2s(us);
            break;
        case timeunit::ms:
            t = us2ms(us);
            break;
        case timeunit::us:
            t = us;
            break;
        default:
            break;
        }
        return t;
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