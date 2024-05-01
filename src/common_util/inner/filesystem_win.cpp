#if defined(_WIN32) || defined(__WIN32__)

#include <io.h>
#include "filesystem.h"
#include "logger.h"

namespace cutl
{

    static constexpr char win_separator = '\\';

    std::string current_program_dir()
    {
        // todo: implement this
        return "";
    }

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

    std::string file_readlink(const std::string &filepath)
    {
        // todo: implement this
        return "";
    }

    bool file_createlink(const std::string &referenece, const std::string &filepath)
    {
        // todo: implement this
        return true;
    }

    bool create_dir(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }
    // remove empty directory
    bool remove_dir(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }
    // remove directory recursively
    bool remove_dir_recursive(const std::string &dir_path)
    {
        // todo: implement this
        return true;
    }

    bool file_sync(FILE *handle)
    {
        return true;
    }

    // get directory size in bytes
    uint64_t get_dirsize(const std::string &dirpath)
    {
        // todo: implement this
        return 0;
    }

    filetype get_file_type(const std::string &filepath)
    {
        // todo: implement this
        return filetype::unknown;
    }

    filevec list_sub_files(const std::string &dirpath, filetype type, bool recursive)
    {
        // todo: implement this
        filevec filelist;
        return filelist;
    }

    bool copy_attributes(const std::string &srcpath, const std::string &dstpath)
    {
        // todo: implement this
        return true;
    }

} // namespace cutl

#endif // defined(_WIN32) || defined(__WIN32__)