#pragma once

#include <string>

namespace cutl
{

#if defined(_WIN32)
#include <windows.h>
using dl_handle_t = HMODULE; /* WIN32 handle type */
#else
using dl_handle_t = void*; /* UNIX handle type */
#endif

class dlloader
{
public:
    dlloader(const std::string& lib_path);
    ~dlloader();

public:
    dl_handle_t get_symbol(const std::string& symbol_name);

private:
    dl_handle_t library_handle_;
};

} // namespace cutl