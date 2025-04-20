#include "timer.h"
#include "inner/logger.h"
#include "threadutil.h"

namespace cutl
{

timer::timer(const std::string& name, Task task, Duration interval)
  : running_(false)
  , name_(name)
  , task_(std::move(task))
  , interval_(interval)
{
}

timer::~timer()
{
    stop();
}

void timer::start()
{
    if (running_.load())
    {
        return;
    }

    // 防止上一次的暂停(stop)还未结束
    if (thread_.joinable())
    {
        thread_.join();
    }

    CUTL_INFO("timer [" + name_ + "] starting...");

    // 启动线程
    running_.store(true);
    thread_ = std::thread(&timer::run, this);

    CUTL_INFO("Timer [" + name_ + "] started.");
}

void timer::stop(bool wait_for_stop)
{
    if (!running_)
    {
        return;
    }

    CUTL_INFO("Timer [" + name_ + "] stoping...");

    // running_标致设置为false, 并通知线程退出
    running_.store(false);
    cv_stopping_.notify_one();
    // cv_stopping_.notify_all();

    // 等待线程退出
    if (wait_for_stop && thread_.joinable())
    {
        thread_.join();
    }

    CUTL_INFO("Timer [" + name_ + "] stoped.");
}

void timer::run()
{
    // 设置线程名称
    set_current_thread_name(name_);

    current_period_start_ = std::chrono::steady_clock::now();

    while (running_.load())
    {
        // 执行任务
        task_();

        // 期望执行的时间
        auto expected_end_time = current_period_start_ + interval_;
        // 任务开始执行时的时间
        auto now = std::chrono::steady_clock::now();
        if (now >= expected_end_time)
        {
            auto delta =
              std::chrono::duration_cast<std::chrono::nanoseconds>(expected_end_time - now)
                .count();
            CUTL_WARN("Timer [" + name_ + "] The current time exceeds the expected time. period:" +
                      std::to_string(interval_.count()) + "us, delta:" + std::to_string(delta) +
                      "us");
            current_period_start_ = now;
            continue;
        }

        // 等待下一此执行
        auto sleep_time = expected_end_time - now;
        // std::this_thread::sleep_for(sleep_time);
        // current_period_start_ = std::chrono::steady_clock::now();

        std::unique_lock<std::mutex> lock(mutex_stopping_);
        if (cv_stopping_.wait_until(
              lock, expected_end_time, [this]() { return !running_.load(); }))
        {
            // 收到了 stop 的消息
            // std::cout << "received stop message" << std::endl;
        }
        else
        {
            // 等待超时，更新下一个执行时间
            current_period_start_ += interval_;
        }
    }
}

} // namespace cutl
