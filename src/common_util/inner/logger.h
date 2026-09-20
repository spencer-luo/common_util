#pragma once

// 库内部头文件，不对外安装。使用方需要的是 logtype.h 里的
// loglevel / LogFuncType，以及 config.h 里的 library_init()。

#include "singleton.h"
#include "logtype.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <sstream>
#include <string>

namespace cutl
{

/**
 * @brief 库内部日志的汇聚点，进程内唯一
 *
 * 库自身不依赖任何日志框架：默认输出到 stdout/stderr，宿主进程通过
 * library_init() 把库日志接到自己的日志系统。
 *
 * 输出函数只在初始化期发布一次，之后 write() 只做一次原子读，不加锁。
 * 日志输出请用 CUTL_DEBUG/CUTL_INFO/CUTL_WARN/CUTL_ERROR 宏。
 */
class logger
{
    CUTL_SINGLETON_REF(logger)
public:
    /**
     * @brief 见 library_init()
     *
     * @return 输出函数是否由本次调用发布；false 表示之前已经设置过
     */
    bool init(LogFuncType log_func, loglevel level = loglevel::info_level);

    loglevel get_level() const;

    /// 该级别是否需要输出。无锁，供日志宏在拼接字符串前快速判断
    bool enabled(loglevel level) const;

    /// 日志宏的输出入口
    void write(loglevel level, const char* file, int line, const char* func, const std::string& msg);

private:
    void default_write(loglevel level, const std::string& msg);

    /// log_func_ 的发布状态，取值转换只允许 unset -> claimed -> ready
    enum class sink_state : uint8_t
    {
        /// 未设置输出函数，走默认实现
        unset,
        /// 某个线程抢到了设置权，正在写 log_func_
        claimed,
        /// log_func_ 已写完，且对读侧可见
        ready,
    };

private:
    std::atomic<loglevel> level_{ loglevel::info_level };
    /// 只在发布时写入一次，此后只读
    LogFuncType log_func_;
    /// 兼作设置权的争夺与 log_func_ 的发布标志，读侧据此判断能否安全读 log_func_
    std::atomic<sink_state> sink_state_{ sink_state::unset };
    /// 仅默认实现使用，保证多线程下整行输出不交错
    std::mutex default_write_mutex_;
};

} // namespace cutl

#define CUTL_LOGGER cutl::logger::get_instance()

#define CUTL_LOG(level, expr)                                                                     \
    do                                                                                            \
    {                                                                                             \
        cutl::logger& cutl_logger__ = CUTL_LOGGER;                                                \
        if (cutl_logger__.enabled((level)))                                                       \
        {                                                                                         \
            std::ostringstream cutl_log_oss__;                                                    \
            cutl_log_oss__ << expr;                                                               \
            cutl_logger__.write((level), __FILE__, __LINE__, __func__, cutl_log_oss__.str());     \
        }                                                                                         \
    } while (0)

#define CUTL_DEBUG(expr) CUTL_LOG(cutl::loglevel::debug_level, expr)
#define CUTL_INFO(expr) CUTL_LOG(cutl::loglevel::info_level, expr)
#define CUTL_WARN(expr) CUTL_LOG(cutl::loglevel::warn_level, expr)
#define CUTL_ERROR(expr) CUTL_LOG(cutl::loglevel::error_level, expr)
