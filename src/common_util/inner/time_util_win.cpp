#if defined(_WIN32)

#include <cstdlib>
#include "system_util.h"
#include "inner/logger.h"

namespace cutl
{

    // 将time_t时间转换为本地时间
    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/localtime-s-localtime32-s-localtime64-s?view=msvc-170
    struct tm localtime_security(const time_t &second)
    {
        struct tm datetime = {0};

        // localtime 线程不安全
        // pDatetime = std::localtime(&t);
        // localtime_r/localtime_s 线程安全
        int ret = localtime_s(&datetime, &second);
        if (ret != 0)
        {
            CUTL_ERROR(std::string("localtime_s failure, error") + strerror(errno));
            return datetime;
        }

        return datetime;
    }

    // 将time_t时间转换为UTC时间
    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/gmtime-gmtime32-gmtime64?view=msvc-170
    struct tm gmtime_security(const time_t &second)
    {
        struct tm datetime = {0};

        // gmtime 线程不安全
        // pDatetime = std::gmtime(&t);
        // gmtime_r/gmtime_s 线程安全
        int ret = gmtime_s(&datetime, &second);
        if (ret != 0)
        {
            CUTL_ERROR(std::string("gmtime_s failure, error") + strerror(errno));
            return datetime;
        }

        return datetime;
    }

} // namespace cutl

#endif // defined(_WIN32)