
// TODO:
#ifdef WIN32

#include <io.h>
#include "filesystem.h"

namespace cutl
{

    static constexpr char win_separator = '\\';

    bool file_exists(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0) == 0);
    }

    bool file_readable(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0x100) == 0);
    }

    bool file_writable(const std::string &filepath)
    {
        return (_access(filepath.c_str(), 0x10) == 0);
    }

    bool file_executable(const std::string &filepath)
    {
        CUTL_WARN("executable() is not supported on Windows");
        return false;
    }

    bool file_sync(FILE *handle)
    {
        return true;
    }

} // namespace cutl

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)