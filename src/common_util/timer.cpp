#include "timer.h"
#include "inner/logger.h"
#include "threadutil.h"

namespace cutl
{

void report_timer_task_exception(const char* source, const char* what)
{
    const char* name = (source != nullptr && source[0] != '\0') ? source : "-";
    if (what != nullptr)
    {
        CUTL_ERROR("Timer [" << name << "] caught exception: " << what);
    }
    else
    {
        CUTL_ERROR("Timer [" << name << "] caught an unknown exception");
    }
}

timer::timer(const std::string& name, Task task, Duration interval)
  : running_(false)
  , name_(name)
  , task_(std::move(task))
  , interval_(interval)
{
}

timer::~timer()
{
    // stop(false) 之后 running_ 已是 false，这里仍要 join 还没退出的线程。
    stop(true);
}

bool timer::on_timer_thread() const
{
    const auto worker = worker_id_.load();
    return worker != std::thread::id() && worker == std::this_thread::get_id();
}

void timer::start()
{
    // 回调里再 start() 会和正在执行的 run() 抢同一条线程。
    if (on_timer_thread())
    {
        CUTL_WARN("Timer [" << name_ << "] start() on the timer thread, ignore it.");
        return;
    }

    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    if (running_.load())
    {
        return;
    }

    // 上一次 stop(false) 留下的线程还在跑，先等它退出再启动下一次。
    if (thread_.joinable())
    {
        thread_.join();
    }

    if (interval_ <= Duration::zero())
    {
        CUTL_ERROR("Timer [" << name_ << "] interval must be positive");
        return;
    }
    if (!task_)
    {
        CUTL_ERROR("timer task is nullptr");
        return;
    }

    CUTL_INFO("timer [" << name_ << "] starting...");

    running_.store(true);
    const bool started = invoke_timer_task(
      name_.c_str(),
      [this]()
      {
          thread_ = std::thread(&timer::run, this);
      });
    if (!started)
    {
        running_.store(false);
        return;
    }

    CUTL_INFO("Timer [" << name_ << "] started.");
}

void timer::stop(bool wait_for_stop)
{
    // 回调里 join 自己会一直等下去。只请求退出，线程函数返回后由外部 join。
    if (on_timer_thread())
    {
        if (running_.exchange(false))
        {
            CUTL_INFO("Timer [" << name_ << "] stopping...");
        }
        cv_stopping_.notify_one();
        if (wait_for_stop)
        {
            CUTL_ERROR("Timer [" << name_ << "] stop() on the timer thread, skip join.");
        }
        return;
    }

    std::lock_guard<std::mutex> lock(lifecycle_mutex_);
    const bool was_running = running_.exchange(false);
    const bool need_join = wait_for_stop && thread_.joinable();
    if (!was_running && !need_join)
    {
        return;
    }

    if (was_running)
    {
        CUTL_INFO("Timer [" << name_ << "] stopping...");
        cv_stopping_.notify_one();
    }

    if (need_join)
    {
        thread_.join();
        CUTL_INFO("Timer [" << name_ << "] stopped.");
    }
}

void timer::run()
{
    worker_id_.store(std::this_thread::get_id());
    // 设置线程名称
    set_current_thread_name(name_);

    current_period_start_ = std::chrono::steady_clock::now();

    while (running_.load())
    {
        invoke_timer_task(name_.c_str(), [this]() { task_(); });

        if (!running_.load())
        {
            break;
        }

        // 期望本周期结束的时间。任务耗时算在这个周期里，未超时就保持原来的相位。
        auto expected_end_time = current_period_start_ + interval_;
        const auto now = std::chrono::steady_clock::now();
        if (now >= expected_end_time)
        {
            const auto delta =
              std::chrono::duration_cast<std::chrono::microseconds>(now - expected_end_time)
                .count();
            CUTL_WARN("Timer [" << name_
                                << "] The current time exceeds the expected time. period:"
                                << interval_.count() << "us, delta:" << delta << "us");
            // 错过的周期不再补跑，从现在起重新等一个完整周期。
            current_period_start_ = now;
            expected_end_time = current_period_start_ + interval_;
        }

        std::unique_lock<std::mutex> lock(mutex_stopping_);
        if (!cv_stopping_.wait_until(
              lock, expected_end_time, [this]() { return !running_.load(); }))
        {
            current_period_start_ += interval_;
        }
    }

    worker_id_.store(std::thread::id());
}

} // namespace cutl
