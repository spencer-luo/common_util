#include "threadutil.h"
#include "inner/logger.h"
#include "strutil.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <pthread.h>
#include <sys/types.h>
#else
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
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
#elif defined(__APPLE__)
    pthread_setname_np(name.c_str());
#else
    // Linux 平台使用 pthread_setname_np 设置线程名称
    if (name.length() > 15)
    {
        CUTL_WARN("The number of characters in the thread name exceeds 15 characters. name: " +
                  name);
    }
    int result = pthread_setname_np(pthread_self(), name.c_str());
    if (result != 0)
    {
        CUTL_ERROR("Failed to set thread name on Linux. name: " + name +
                   ", result:" + std::to_string(result));
    }
#endif
}

std::string get_current_thread_name()
{
#if defined(_WIN32)
    // Windows 平台使用 GetThreadDescription 获取线程名称
    std::wstring wname;
    HRESULT hr = GetThreadDescription(GetCurrentThread(), &wname);
    if (!SUCCEEDED(hr))
    {
        auto dwErr = GetLastError();
        CUTL_ERROR("get thread name failed. errMsg:" + std::to_string(dwErr));
        return {};
    }
    std::string name = ws2s(wname);
    return name;
#else
    char thread_name[16]{};
    int ret = pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
    if (ret != 0)
    {
        CUTL_ERROR("get thread name failed. ret:" + std::to_string(ret));
        return {};
    }

    return std::string(thread_name);
#endif
}

// 获取当前线程的线程ID，调用本地系统的接口获取短ID
int32_t get_current_thread_tid()
{
    // 使用tls确保每个线程只会执行一次syscall获取线程ID
    static thread_local int32_t thread_id = 0;
    if (thread_id != 0)
    {
        return thread_id;
    }

#if defined(_WIN32)
    thread_id = static_cast<int32_t>(GetCurrentThreadId());
#elif defined(__APPLE__)
    pthread_threadid_np(pthread_self(), &thread_id);
#elif defined(__linux__) || defined(__unix__)
    thread_id = static_cast<int32_t>(syscall(__NR_gettid));
#else
    // 通用后备方案（原子计数器）
    static std::atomic<int32_t> counter(0);
    thread_id = ++counter;
#endif

    return thread_id;
}

} // namespace cutl
