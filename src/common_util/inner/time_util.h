#pragma once

#include <ctime>

namespace cutl
{

    // 将time_t时间转换为本地时间
    struct tm localtime_security(const time_t &second);
    // 将time_t时间转换为UTC时间
    struct tm gmtime_security(const time_t &second);

} // namespace cutl