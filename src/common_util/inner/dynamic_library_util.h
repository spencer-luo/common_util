#pragma once

#include <string>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace cutl
{

#ifdef WIN32
typedef HMODULE handle_t; /* WIN32 handle type */
#else
typedef void* handle_t; /* UNIX handle type */
#endif

handle_t load_library(const std::string& libname);
void* get_symbol_address(handle_t hDll, const std::string& strProcName);
int free_library(handle_t& hDll);
std::string get_last_error();

} // namespace cutl