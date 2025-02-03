#if defined(_WIN32)
// do nothing
#else

#include "dynamic_library_util.h"

namespace cutl
{

dl_handle_t load_library(const std::string& libname)
{
    return dlopen(libname.c_str(), RTLD_LAZY);
}

void* get_symbol_address(dl_handle_t hDll, const std::string& symbol_name)
{
    return dlsym(hDll, symbol_name.c_str());
}

int free_library(dl_handle_t hDll)
{
    return dlclose(hDll);
}

std::string get_last_error()
{
    return std::string(dlerror());
}

} // namespace cutl

#endif // defined(_WIN32)