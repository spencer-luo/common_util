#include "threadutil.h"
#include "inner/logger.h"
#include "strutil.h"

// #include <iostream>
// #include <string>
// #include <thread>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace cutl
{

// 设置当前线程的线程名称
void set_current_thread_name(const std::string& name)
{
#if defined(_WIN32)
    std::wstring wname = s2ws(name);
    // Windows 平台使用 SetThreadDescription 设置线程名称
    HRESULT hr = SetThreadDescription(GetCurrentThread(), wname.c_str());
    if (FAILED(hr))
    {
        CUTL_ERROR("Failed to set thread name on Windows. name" + name);
    }
#else
    // Linux 平台使用 pthread_setname_np 设置线程名称
    int result = pthread_setname_np(pthread_self(), name.c_str());
    if (result != 0)
    {
        CUTL_ERROR("Failed to set thread name on Linux. name" + name);
    }
#endif
}

} // namespace cutl
