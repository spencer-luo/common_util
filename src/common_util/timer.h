#pragma once

#include "inner/logger.h"
#include "threadutil.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
// #include <unique_lock>
#include <utility>

namespace cutl
{

template<typename Callable, typename... Args>
void set_timeout(Callable&& f, uint32_t delay_ms, Args&&... args)
{
    // 将函数和参数绑定，并移动到线程中
    auto task = std::bind(std::forward<Callable>(f), std::forward<Args>(args)...);

    std::thread(
      [delay_ms, task]() mutable
      {
          // 等待指定延迟
          std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

          // 执行目标函数
          task();
      })
      .detach(); // 分离线程，让其独立运行
}

class timer
{
public:
    using Task = std::function<void()>;
    // 微秒
    using Duration = std::chrono::microseconds;

    timer(const std::string& name, Task task, Duration interval);

    ~timer();

    void start();

    void stop();

    bool is_running() const { return running_.load(); }

private:
    void run();

    std::atomic_bool running_{ false };
    std::string name_;
    Task task_;
    std::chrono::steady_clock::time_point current_period_start_;
    Duration interval_;
    std::thread thread_;
    std::mutex mutex_stopping_;
    std::condition_variable cv_stopping_;
};

} // namespace cutl
