#if defined(_WIN32) || defined(__WIN32__)
// do nothing
#else

#include "system_util.h"

namespace cutl
{

    bool call_system(const std::string &cmd)
    {
        if (cmd.empty())
        {
            CUTL_ERROR("cmd is empty!");
            return false;
        }

        pid_t status;
        status = std::system(cmd.c_str());

        if (-1 == status)
        {
            CUTL_ERROR("system error!");
            return false;
        }

        if (!WIFEXITED(status))
        {
            CUTL_ERROR("exit status:" + std::to_string(WEXITSTATUS(status)));
            return false;
        }

        if (0 != WEXITSTATUS(status))
        {
            CUTL_ERROR("run shell script fail, script exit code:" + std::to_string(WEXITSTATUS(status)));
            return false;
        }

        return true;
    }

    FILE *pipline_open(const std::string &cmd)
    {
        return popen(cmd.c_str(), "r");
    }

    int pipline_close(FILE *stream)
    {
        return pclose(stream);
    }

} // namespace cutl

#endif // defined(_WIN32) || defined(__WIN32__)