#if defined(_WIN32) || defined(__WIN32__)
// do nothing
#else

#include <cstdlib>
#include "time_util.h"
#include "inner/logger.h"

namespace cutl
{

    // 将time_t时间转换为本地时间
    struct tm localtime_security(const std::time_t &second)
    {
        struct tm datetime = {0};

        // localtime 线程不安全
        // pDatetime = std::localtime(&t);
        // localtime_r/localtime_s 线程安全
        struct tm *pDatetime = localtime_r(&second, &datetime);

        if (!pDatetime)
        {
            CUTL_ERROR("pDatetime is null");
            return datetime;
        }

        return datetime;
    }

    // 将time_t时间转换为UTC时间
    struct tm gmtime_security(const std::time_t &second)
    {
        struct tm datetime = {0};

        // gmtime 线程不安全
        // pDatetime = std::gmtime(&t);
        // gmtime_r/gmtime_s 线程安全
        struct tm *pDatetime = gmtime_r(&second, &datetime);

        if (!pDatetime)
        {
            CUTL_ERROR("pDatetime is null");
            return datetime;
        }

        return datetime;
    }
} // namespace cutl

#endif // defined(_WIN32) || defined(__WIN32__)