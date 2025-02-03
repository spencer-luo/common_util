#pragma once

#include "dlloader.h"
#include <string>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace cutl
{

dl_handle_t load_library(const std::string& libname);
void* get_symbol_address(dl_handle_t hDll, const std::string& strProcName);
int free_library(dl_handle_t hDll);
std::string get_last_error();

} // namespace cutl