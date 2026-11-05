#include "eventloop.h"
#include "inner/logger.h"
#include "threadutil.h"

#include <exception>

namespace cutl
{

struct TimerTask
{
    TimerTask(const std::string& name,
              const EventloopTimePoint& next_run_time,
              const EventloopDuration& period,
              const EventloopTask& func,
              int64_t repeat_times)
      : name_(name)
      , next_run_time_(next_run_time)
      , period_(period)
      , func_(std::move(func))
      , left_times_(repeat_times)
    {
    }

    bool is_valid() const { return left_times_.load() != 0; }

    void cancel()
    {
        left_times_.store(0);
        CUTL_WARN("TimerTask[" << name_ << "] is canceled.");
    }

    void update_left_times()
    {
        // cancel() 可能同时把次数写成 0。必须一次交换完成，避免读到旧值后写成 -1。
        int64_t times = left_times_.load();
        while (times > 0 && !left_times_.compare_exchange_weak(times, times - 1))
        {
        }
    }

    void update_next_run_time(EventloopTimePoint now)
    {
        next_run_time_ += period_;
        if (next_run_time_ < now)
        {
            // 当前时间超出期望(的执行)时间至少一个周期，则调整期望的执行时间
            if (now - next_run_time_ > period_)
            {
                auto delta =
                  std::chrono::duration_cast<std::chrono::nanoseconds>(next_run_time_ - now)
                    .count();
                int period_ns = period_.count();
                CUTL_WARN("TimerTask[" << name_ <<
                          "] The current time exceeds the expected time by at least one period_[" <<
                          period_ns << "ns]. Delta: " << delta <<
                          "ns");

                next_run_time_ = now;
            }
        }
    }

    bool operator<(const TimerTask& other) const { return next_run_time_ < other.next_run_time_; }

    // 名称
    std::string name_;
    // 触发时间
    EventloopTimePoint next_run_time_;
    // 任务的轮训周期
    EventloopDuration period_;
    // 任务
    EventloopTask func_;
    // 剩余的执行次数（-1: 表示无限循环）
    std::atomic<int64_t> left_times_;
};

// 定时任务队列的比较函数: 定义最小堆
bool TimerTaskCompare(const TimerTaskPtr& a, const TimerTaskPtr& b)
{
    // a > b: 表示最小堆
    return *b < *a;
}

timer_task_handler::timer_task_handler(const TimerTaskPtr& timer_task)
  : timer_task_(timer_task)
{
}

timer_task_handler::timer_task_handler(timer_task_handler&& other) noexcept
  : timer_task_(other.timer_task_)
{
    other.timer_task_.reset();
}

timer_task_handler& timer_task_handler::operator=(timer_task_handler&& other) noexcept
{
    cancel();

    timer_task_ = other.timer_task_;
    other.timer_task_.reset();
    return *this;
}

void timer_task_handler::cancel()
{
    auto p = timer_task_.lock();
    if (p)
    {
        p->cancel();
    }
}

/*
 * 定时任务失效或销毁的几种情况：
 * 1. TimerTask 被标记为取消(repeat设置为0), 任务还在执行中还未被销毁。
 * 2. TimerTask 指定次数的任务执行完毕, 任务还在执行中还未被销毁。
 * 3. TimerTask 执行完毕，对象已经被销毁。
 */
bool timer_task_handler::isvalid()
{
    auto p = timer_task_.lock();
    return p && p->is_valid();
}

eventloop::eventloop(uint32_t task_max_size, uint32_t timer_task_max_size)
  : is_running_(false)
  , loop_thread_id_()
  , task_queue_()
  , task_max_size_(task_max_size)
  , timer_task_mutex_()
  , timer_task_queue_(&TimerTaskCompare)
  , timer_task_max_size_(timer_task_max_size)
  , timer_task_inflight_(0)
  , wakeup_pending_(false)
  , in_loop_(false)
{
}

eventloop::~eventloop()
{
    stop();
    wait_loop_finished();
    clear_pending_tasks();
}

bool eventloop::post_event(const EventloopTask& task)
{
    bool empty = true;
    {
        std::lock_guard<std::mutex> guard(task_mutex_);
        if (task_queue_.size() >= task_max_size_)
        {
            CUTL_ERROR(
              "Task queue is full, discard task. size:" << task_queue_.size() <<
              ", max_size:" << task_max_size_);
            return false;
        }
        empty = task_queue_.empty();
        task_queue_.emplace_back(task);
    }

    // 如果该任务是第一个任务，唤醒Loop线程进行处理
    if (empty)
    {
        wakeup();
    }

    return true;
}

void eventloop::start()
{
    if (is_running_.load())
    {
        // 重复调用
        CUTL_WARN("recursive call of start.");
        return;
    }

    start_loop();
}

bool eventloop::is_loop_thread() const
{
    return loop_thread_id_ == std::this_thread::get_id();
}

void eventloop::wakeup()
{
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        wakeup_pending_ = true;
    }
    cv_wakeup_.notify_one();
}

void eventloop::wait_for_timeout_or_wakeup(std::chrono::microseconds timeout)
{
    if (timeout < std::chrono::microseconds::zero())
    {
        timeout = std::chrono::microseconds::zero();
    }

    std::unique_lock<std::mutex> lock(cv_mutex_);
    EventloopTimePoint abs_timeout = std::chrono::steady_clock::now() + timeout;
    // 谓词和 wakeup_pending_ 共用 cv_mutex_，notify 发生在进入等待之前也不会丢。
    const bool signaled =
      cv_wakeup_.wait_until(lock, abs_timeout, [this]() { return wakeup_pending_; });
    wakeup_pending_ = false;
    if (signaled)
    {
        CUTL_DEBUG("Condition met!");
    }
    else
    {
        CUTL_DEBUG("Timeout occurred!");
    }
}

void eventloop::wait_loop_finished()
{
    if (is_loop_thread())
    {
        return;
    }

    std::unique_lock<std::mutex> lock(cv_mutex_);
    cv_wakeup_.wait(lock, [this]() { return !in_loop_; });
}

void eventloop::clear_pending_tasks()
{
    std::list<EventloopTask> tasks;
    {
        std::lock_guard<std::mutex> guard(task_mutex_);
        tasks.swap(task_queue_);
    }

    decltype(timer_task_queue_) timers(&TimerTaskCompare);
    {
        std::lock_guard<std::mutex> lock(timer_task_mutex_);
        timer_task_queue_.swap(timers);
        timer_task_inflight_ = 0;
    }
}

void eventloop::stop()
{
    is_running_.store(false);
    wakeup();
}

timer_task_handler eventloop::post_timer_event(const std::string& name,
                                               const EventloopTask& func,
                                               const EventloopDuration& period,
                                               int64_t repeat)
{
    auto handler = post_to_priorityqueue(name, func, period, repeat);
    if (handler.isvalid())
    {
        wakeup();
    }
    return handler;
}

void eventloop::loop_once(EventloopDuration timeout)
{
    // 处理定时任务
    size_t timer_task_done = handle_timer_task();
    // CUTL_DEBUG(timer_task_done << " timer task(s) done.");

    // 处理普通任务
    size_t task_done = handle_task();
    // CUTL_DEBUG(task_done << " task(s) done.");

    // 从定时任务队列中获取离当前时间最近的定时任务的到期间隔
    EventloopDuration next_timer_task_duration = get_next_run_time();
    if (next_timer_task_duration < timeout)
    {
        timeout = next_timer_task_duration;
    }

    wait_for_timeout_or_wakeup(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
}

namespace
{

void invoke_event_task(const EventloopTask& task)
{
    if (!task)
    {
        CUTL_WARN("eventloop task is nullptr");
        return;
    }

    try
    {
        task();
    }
    catch (const std::exception& e)
    {
        CUTL_ERROR("eventloop task threw: " << e.what());
    }
    catch (...)
    {
        CUTL_ERROR("eventloop task threw an unknown exception");
    }
}

} // namespace

size_t eventloop::handle_task()
{
    std::list<EventloopTask> tasks;
    {
        std::lock_guard<std::mutex> guard(task_mutex_);
        // swap后，task_queue_变成了空队列，tasks获取到了所有队列元素
        tasks.swap(task_queue_);
    }

    for (const auto& task : tasks)
    {
        invoke_event_task(task);
    }
    return tasks.size();
}

size_t eventloop::handle_timer_task()
{
    EventloopTimePoint now = std::chrono::steady_clock::now();
    // 一次性取出所有就绪的定时任务
    TimerTaskVec ready_tasks = get_expired_timer_tasks(now);

    // 执行就绪的定时任务
    size_t done = 0;
    for (auto& task : ready_tasks)
    {
        if (!task->is_valid())
        {
            continue;
        }

        // CUTL_INFO("[" << task->name_ << "]left_times:" << task->left_times_.load());
        invoke_event_task(task->func_);
        task->update_left_times();
        ++done;
    }

    // 再获取一次 now，因为上面可能比较耗时
    now = std::chrono::steady_clock::now();
    restore_timer_tasks(ready_tasks, now);
    return done;
}

void eventloop::start_loop()
{
    loop_thread_id_ = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        in_loop_ = true;
        is_running_.store(true);
    }

    try
    {
        while (is_running_.load())
        {
            loop_once(std::chrono::milliseconds(500));
        }
    }
    catch (const std::exception& e)
    {
        CUTL_ERROR("eventloop loop threw: " << e.what());
        is_running_.store(false);
    }
    catch (...)
    {
        CUTL_ERROR("eventloop loop threw an unknown exception");
        is_running_.store(false);
    }
    end_loop();
}

void eventloop::end_loop()
{
    {
        std::lock_guard<std::mutex> lock(cv_mutex_);
        in_loop_ = false;
    }
    cv_wakeup_.notify_all();
}

eventloop::TimerTaskVec eventloop::get_expired_timer_tasks(EventloopTimePoint now)
{
    std::vector<TimerTaskPtr> ready_tasks;
    std::lock_guard<std::mutex> lock(timer_task_mutex_);
    while (!timer_task_queue_.empty())
    {
        TimerTaskPtr timerTask = timer_task_queue_.top();

        // auto task_run_time = fmt_timepoint(timerTask->next_run_time_);
        // auto not_time = fmt_timepoint(now);
        // CUTL_INFO("[" << timerTask->name_ << " : " << count
        //           << "] run_time: " << task_run_time << ", now_time:" << not_time);

        // 最小堆的堆顶元素(任务)的执行时间大于当前时间，说明堆内所有任务均未就绪(未到执行时间)
        if (timerTask->next_run_time_ > now)
        {
            break;
        }
        timer_task_queue_.pop();
        ready_tasks.emplace_back(std::move(timerTask));
    }
    timer_task_inflight_ += ready_tasks.size();
    return ready_tasks;
}

void eventloop::restore_timer_tasks(TimerTaskVec& ready_tasks, EventloopTimePoint now)
{
    std::lock_guard<std::mutex> guard(timer_task_mutex_);
    if (timer_task_inflight_ >= ready_tasks.size())
    {
        timer_task_inflight_ -= ready_tasks.size();
    }
    else
    {
        timer_task_inflight_ = 0;
    }

    for (auto& task : ready_tasks)
    {
        if (!task->is_valid())
        {
            continue;
        }
        task->update_next_run_time(now);
        timer_task_queue_.emplace(std::move(task));
    }
}

timer_task_handler eventloop::post_to_priorityqueue(const std::string& name,
                                                    const EventloopTask& func,
                                                    const EventloopDuration& period,
                                                    int64_t repeat)
{
    auto now = std::chrono::steady_clock::now();
    auto task = std::make_shared<TimerTask>(name, now + period, period, func, repeat);

    std::lock_guard<std::mutex> lock(timer_task_mutex_);
    if (timer_task_queue_.size() + timer_task_inflight_ >= timer_task_max_size_)
    {
        CUTL_ERROR("Timer task queue is full, discard task. size:" << timer_task_queue_.size()
                                                                    << ", inflight:"
                                                                    << timer_task_inflight_
                                                                    << ", max_size:"
                                                                    << timer_task_max_size_);
        return timer_task_handler(nullptr);
    }
    timer_task_queue_.emplace(task);
    return cutl::timer_task_handler(task);
}

EventloopDuration eventloop::get_next_run_time()
{
    // using namespace std::chrono_literals;
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timer_task_mutex_);
    if (timer_task_queue_.empty())
    {
        return std::chrono::seconds(1);
    }

    if (timer_task_queue_.top()->next_run_time_ <= now)
    {
        return EventloopDuration::zero();
    }
    return timer_task_queue_.top()->next_run_time_ - now;
}

singlethread_eventloop::singlethread_eventloop(const std::string thread_name,
                                               uint32_t task_max_size,
                                               uint32_t timer_task_max_size)
  : eventloop(task_max_size, timer_task_max_size)
  , thread_name_(thread_name)
{
}

void singlethread_eventloop::start()
{
    if (is_running_.load())
    {
        // 重复调用
        CUTL_WARN("recursive call of start.");
        return;
    }

    // 开启一个独立的字线程来执行事件循环的任务
    loop_thread_ = std::thread(
      [this]()
      {
          cutl::set_current_thread_name(thread_name_);
          start_loop();
      });
}

singlethread_eventloop::~singlethread_eventloop()
{
    stop();
    clear_pending_tasks();
}

void singlethread_eventloop::stop()
{
    // 执行父类的stop函数
    eventloop::stop();

    // 任务回调里调用 stop() 时，当前线程就是循环线程，join 自己会死锁。
    if (loop_thread_.get_id() == std::this_thread::get_id())
    {
        CUTL_ERROR("singlethread_eventloop stop() on the loop thread skips join");
        return;
    }

    // 等待线程退出
    if (loop_thread_.joinable())
    {
        loop_thread_.join();
    }
}

multithread_eventloop::multithread_eventloop(uint32_t task_max_size,
                                             uint32_t timer_task_max_size,
                                             uint32_t thread_num)
  : eventloop(task_max_size, timer_task_max_size)
  , thread_pool_("multi-evtloop", task_max_size + timer_task_max_size)
  , thread_num_(thread_num)
{
}
multithread_eventloop::~multithread_eventloop()
{
    eventloop::stop();
    wait_loop_finished();
    thread_pool_.shutdown(false);
    clear_pending_tasks();
}

void multithread_eventloop::start()
{
    if (is_running_.load())
    {
        // 重复调用
        CUTL_WARN("recursive call of start.");
        return;
    }

    thread_pool_.start(thread_num_);
    start_loop();
}

void multithread_eventloop::stop()
{
    eventloop::stop();
    thread_pool_.stop();
}

size_t multithread_eventloop::handle_task()
{
    std::list<EventloopTask> tasks;
    {
        std::lock_guard<std::mutex> guard(task_mutex_);
        // swap后，task_queue_变成了空队列，tasks获取到了所有队列元素
        tasks.swap(task_queue_);
    }

    for (const auto& task : tasks)
    {
        thread_pool_.add_task(task);
    }
    return tasks.size();
}

size_t multithread_eventloop::handle_timer_task()
{
    EventloopTimePoint now = std::chrono::steady_clock::now();
    // 一次性取出所有就绪的定时任务
    TimerTaskVec ready_tasks = get_expired_timer_tasks(now);

    // 执行就绪的定时任务
    size_t done = 0;
    for (auto& task : ready_tasks)
    {
        if (!task->is_valid())
        {
            continue;
        }

        thread_pool_.add_task(task->func_);
        task->update_left_times();
        ++done;
    }

    // 再获取一次 now，因为上面可能比较耗时
    now = std::chrono::steady_clock::now();
    restore_timer_tasks(ready_tasks, now);
    return done;
}

} // namespace cutl
