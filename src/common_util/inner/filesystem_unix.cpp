// TODO:
#ifndef WIN32

#include <unistd.h>
#include <sys/stat.h>
#include "filesystem.h"
#include "inner/logger.h"

namespace cutl
{
    // https://blog.csdn.net/giveaname/article/details/88973102
    std::string current_program_dir()
    {
        constexpr int size = 1024;
        char buffer[size] = {0};
        char *presult = getcwd(buffer, size);
        if (nullptr == presult)
        {
            CUTL_ERROR("presult is nullptr");
            return "";
        }
        return std::string(presult);
    }

    // https://blog.csdn.net/venom_snake/article/details/88066475
    bool create_dir(const std::string &dir_path)
    {
        if (0 != mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO | S_IWOTH))
        {
            CUTL_ERROR("mkdir error. dir:" + dir_path + ", error:" + strerror(errno));
            return false;
        }
        return true;
    }

    bool remove_dir(const std::string &dir_path)
    {
        return false;
    }

} // namespace cutl

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)