#include "logger.h"
#include "time_util.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <thread>
#include <utility>

namespace cutl
{

namespace
{

const char* level_flag(loglevel level)
{
    switch (level)
    {
        case loglevel::debug_level:
            return "[D]";
        case loglevel::info_level:
            return "[I]";
        case loglevel::warn_level:
            return "[W]";
        case loglevel::error_level:
            return "[E]";
    }
    return "[?]";
}

/// 只取文件名，避免日志里出现编译机上的绝对路径
std::string base_name(const char* path)
{
    std::string file(path ? path : "");
    const size_t pos = file.find_last_of("/\\");
    return pos == std::string::npos ? file : file.substr(pos + 1);
}

/// 当前本地时间，形如 2026-09-07 19:53:21.123
std::string local_time_text()
{
    const auto now = std::chrono::system_clock::now();
    const auto whole_sec = std::chrono::time_point_cast<std::chrono::seconds>(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - whole_sec);

    const std::time_t secs = std::chrono::system_clock::to_time_t(whole_sec);
    const std::tm local = localtime_security(secs);

    char text[32];
    const size_t len = std::strftime(text, sizeof(text), "%Y-%m-%d %H:%M:%S", &local);
    if (len == 0)
    {
        return "-";
    }
    std::snprintf(text + len, sizeof(text) - len, ".%03d", static_cast<int>(ms.count()));
    return text;
}

struct reentrancy_guard
{
    bool& flag;
    explicit reentrancy_guard(bool& f)
      : flag(f)
    {
        flag = true;
    }
    ~reentrancy_guard() { flag = false; }
};

} // namespace

logger::logger() = default;

logger::~logger() = default;

bool logger::init(LogFuncType log_func, loglevel level)
{
    level_.store(level, std::memory_order_relaxed);
    if (!log_func)
    {
        // 沿用默认实现，不消耗这次一次性的发布机会
        return true;
    }

    // 输出函数发布后不再替换：正在打日志的线程可能正在读它，改写会让它读到
    // 构造中途的对象。反正这是初始化期的一次性设置。
    // 这里的 CAS 只用来决出唯一的设置者，跨线程的可见性由下面的 release 存储建立。
    sink_state expected = sink_state::unset;
    if (!sink_state_.compare_exchange_strong(
          expected, sink_state::claimed, std::memory_order_relaxed))
    {
        return false;
    }

    log_func_ = std::move(log_func);
    // release 保证读侧看到 ready 时，log_func_ 的写入已经可见
    sink_state_.store(sink_state::ready, std::memory_order_release);
    return true;
}

loglevel logger::get_level() const
{
    return level_.load(std::memory_order_relaxed);
}

bool logger::enabled(loglevel level) const
{
    return static_cast<int>(level) >= static_cast<int>(level_.load(std::memory_order_relaxed));
}

void logger::write(loglevel level,
                   const char* file,
                   int line,
                   const char* func,
                   const std::string& msg)
{
    if (!enabled(level))
    {
        return;
    }

    std::ostringstream text;
    text << "[" << base_name(file) << ":" << line << ":" << (func ? func : "-") << "] "
        << msg;

    if (sink_state_.load(std::memory_order_acquire) == sink_state::ready)
    {
        log_func_(level, text.str());
    }
    else
    {
        default_write(level, text.str());
    }
}

void logger::default_write(loglevel level, const std::string& msg)
{
    // localtime_security() 失败时会再打日志，避免默认输出路径递归
    static thread_local bool reentrant = false;
    std::ostream& out = level == loglevel::error_level ? std::cerr : std::cout;
    if (reentrant)
    {
        std::lock_guard<std::mutex> lock(default_write_mutex_);
        out << msg << std::endl;
        return;
    }

    reentrancy_guard guard(reentrant);
    const std::string time_text = local_time_text();
    std::lock_guard<std::mutex> lock(default_write_mutex_);
    out << "[" << time_text << "][" << level_flag(level) << "][" << std::this_thread::get_id()
        << "] " << msg << std::endl;
}

} // namespace cutl
