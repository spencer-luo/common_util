#pragma once

#include <stdint.h>

namespace cutl
{

    // 获取当前时间戳:秒
    uint64_t timestamp();
    // 获取当前时间戳:毫秒
    uint64_t timestamp_ms();
    // 获取当前时间戳:微秒
    uint64_t timestamp_us();
    // 获取系统启动到当前的时间，单位：秒
    uint64_t clocktime();
    // 获取系统启动到当前的时间，单位：毫秒
    uint64_t clocktime_ms();
    // 获取系统启动到当前的时间，单位：微秒
    uint64_t clocktime_us();

    // 时间单位转换： 毫秒-->秒
    uint64_t ms2s(uint64_t time);
    uint64_t us2s(uint64_t time);
    uint64_t us2ms(uint64_t time);
    uint64_t s2ms(uint64_t time);
    uint64_t s2us(uint64_t time);
    uint64_t ms2us(uint64_t time);

} // namespace