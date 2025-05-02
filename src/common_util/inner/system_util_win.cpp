#if defined(_WIN32)

#include <cstdlib>
#include "system_util.h"
#include "inner/logger.h"

namespace cutl
{

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/system-wsystem?view=msvc-170
    bool call_system(const std::string &cmd)
    {
        if (cmd.empty())
        {
            CUTL_ERROR("cmd is empty!");
            return false;
        }

        int ret = system(cmd.c_str());
        if (ret != 0)
        {
            CUTL_ERROR(std::string("system failure, error") + strerror(errno));
            return false;
        }

        return true;
    }

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/popen-wpopen?view=msvc-170
    FILE *pipline_open(const std::string &cmd)
    {
        return _popen(cmd.c_str(), "r");
    }

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/pclose?view=msvc-170
    int pipline_close(FILE *stream)
    {
        return _pclose(stream);
    }

} // namespace cutl

#endif // defined(_WIN32)