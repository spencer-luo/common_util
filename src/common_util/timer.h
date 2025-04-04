#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
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

} // namespace cutl
