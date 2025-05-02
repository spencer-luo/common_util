#include "eventloop.h"
#include "inner/logger.h"

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
        CUTL_WARN("TimerTask[" + name_ + "] is canceled.");
    }

    void update_left_times()
    {
        if (left_times_.load() > 0)
        {
            left_times_.store(left_times_.load() - 1);
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
                CUTL_WARN("TimerTask[" + name_ +
                          "] The current time exceeds the expected time by at least one period_[" +
                          std::to_string(period_ns) + "ns]. Delta: " + std::to_string(delta) +
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
{
}

eventloop::~eventloop() {}

bool eventloop::post_event(const EventloopTask& task)
{
    bool empty = true;
    {
        std::lock_guard<std::mutex> guard(task_mutex_);
        if (task_queue_.size() >= task_max_size_)
        {
            CUTL_ERROR(
              "Task queue is full, discard task. size:" + std::to_string(task_queue_.size()) +
              ", max_size:" + std::to_string(task_max_size_));
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

    is_running_.store(true);
    loop_thread_id_ = std::this_thread::get_id();

    while (is_running_.load())
    {
        loop_once(std::chrono::milliseconds(500));
    }
}

bool eventloop::is_loop_thread() const
{
    return loop_thread_id_ == std::this_thread::get_id();
}

void eventloop::wakeup()
{
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
    if (cv_wakeup_.wait_until(lock, abs_timeout) == std::cv_status::timeout)
    {
        CUTL_DEBUG("Timeout occurred!");
    }
    else
    {
        CUTL_DEBUG("Condition met!");
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
    {
        std::lock_guard<std::mutex> lock(timer_task_mutex_);
        if (timer_task_queue_.size() >= timer_task_max_size_)
        {
            CUTL_ERROR("Timer task queue is full, discard task. size:" +
                       std::to_string(timer_task_queue_.size()) +
                       ", max_size:" + std::to_string(timer_task_max_size_));
            return timer_task_handler(nullptr);
        }
    }
    auto handler = post_to_priorityqueue(name, func, period, repeat);
    wakeup();
    return handler;
}

size_t eventloop::test_get_task_size()
{
    std::lock_guard<std::mutex> lg(task_mutex_);
    return task_queue_.size();
}

void eventloop::loop_once(EventloopDuration timeout)
{
    // 处理定时任务
    size_t timer_task_done = handle_timer_task();
    // CUTL_DEBUG(std::to_string(timer_task_done) + " timer task(s) done.");

    // 处理普通任务
    size_t task_done = handle_task();
    // CUTL_DEBUG(std::to_string(task_done) + " task(s) done.");

    // 从定时任务队列中获取离当前时间最近的定时任务的到期间隔
    EventloopDuration next_timer_task_duration = get_next_run_time();
    if (next_timer_task_duration < timeout)
    {
        timeout = next_timer_task_duration;
    }

    wait_for_timeout_or_wakeup(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
}

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
        task();
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

        // CUTL_INFO("[" + task->name_ + "]left_times:" +
        // std::to_string(task->left_times_.load()));
        task->func_();
        task->update_left_times();
        ++done;
    }

    // 再获取一次 now，因为上面可能比较耗时
    now = std::chrono::steady_clock::now();

    // 任务执行完后再将有效的定时任务重新放入优先队列
    std::lock_guard<std::mutex> guard(timer_task_mutex_);
    for (auto& task : ready_tasks)
    {
        if (!task->is_valid())
        {
            continue;
        }
        task->update_next_run_time(now);
        timer_task_queue_.emplace(task);
    }

    return done;
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
        // CUTL_INFO("[" + timerTask->name_ + " : " + std::to_string(count) +
        //           "] run_time: " + task_run_time + ", now_time:" + not_time);

        // 最小堆的堆顶元素(任务)的执行时间大于当前时间，说明堆内所有任务均未就绪(未到执行时间)
        if (timerTask->next_run_time_ > now)
        {
            break;
        }
        timer_task_queue_.pop();
        ready_tasks.emplace_back(std::move(timerTask));
    }
    return ready_tasks;
}

timer_task_handler eventloop::post_to_priorityqueue(const std::string& name,
                                                    const EventloopTask& func,
                                                    const EventloopDuration& period,
                                                    int repeat)
{

    auto now = std::chrono::steady_clock::now();
    auto task = std::make_shared<TimerTask>(name, now + period, period, func, repeat);

    std::lock_guard<std::mutex> lock(timer_task_mutex_);
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

multithread_eventloop::multithread_eventloop(uint32_t task_max_size,
                                             uint32_t timer_task_max_size,
                                             uint32_t thread_num)
  : eventloop(task_max_size, timer_task_max_size)
  , thread_pool_("multi-evtloop", task_max_size + timer_task_max_size)
  , thread_num_(thread_num)
{
}
multithread_eventloop::~multithread_eventloop() {}

void multithread_eventloop::start()
{
    thread_pool_.start(thread_num_);
    eventloop::start();
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

    // 任务执行完后再将有效的定时任务重新放入优先队列
    std::lock_guard<std::mutex> guard(timer_task_mutex_);
    for (auto& task : ready_tasks)
    {
        if (!task->is_valid())
        {
            continue;
        }
        task->update_next_run_time(now);
        timer_task_queue_.emplace(task);
    }

    return done;
}

} // namespace cutl
