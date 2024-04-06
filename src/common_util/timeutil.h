#pragma once

#include <cstdint>

namespace cutl
{

    enum class timeunit
    {
        s,
        ms,
        us,
    };

    // 获取当前时间戳:秒
    uint64_t timestamp(timeunit unit);
    // 获取系统启动到当前的时间，单位：秒
    uint64_t clocktime(timeunit unit);

    // 时间单位转换： 毫秒-->秒
    uint64_t ms2s(uint64_t time);
    uint64_t us2s(uint64_t time);
    uint64_t us2ms(uint64_t time);
    uint64_t s2ms(uint64_t time);
    uint64_t s2us(uint64_t time);
    uint64_t ms2us(uint64_t time);

} // namespace