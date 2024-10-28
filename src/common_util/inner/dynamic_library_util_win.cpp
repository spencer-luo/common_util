#if defined(_WIN32)

#include "dynamic_library_util.h"
#include "strutil.h"
#include <strstream>

namespace cutl
{

dl_handle_t load_library(const std::string& libname)
{
    auto strLibname = s2ws(libname);
    return LoadLibrary(strLibname.c_str());
}

void* get_symbol_address(dl_handle_t hDll, const std::string& symbol_name)
{
    return GetProcAddress(hDll, symbol_name.c_str());
}

int free_library(dl_handle_t hDll)
{
    return FreeLibrary(hDll);
}

std::string get_last_error()
{
    // 将返回的DWORD类型转换成std::string类型
    auto dwErr = GetLastError();
    return ws2s(std::wstring(dwErr));
}

} // namespace cutl

#endif // defined(_WIN32)