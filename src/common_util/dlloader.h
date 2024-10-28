#pragma once

#include <map>
#include <string>

namespace cutl
{

#if defined(_WIN32)
#include <windows.h>
typedef HMODULE dl_handle_t; /* WIN32 handle type */
#else
typedef void* dl_handle_t; /* UNIX handle type */
#endif

class dlloader
{
public:
    dlloader(const std::string& lib_path);
    ~dlloader();

public:
    dl_handle_t get_symbol(const std::string& symbol_name);

private:
    std::map<std::string, void*> symbol_map_;
};

} // namespace cutl