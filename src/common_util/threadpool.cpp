#include "threadpool.h"
#include "inner/logger.h"
#include "threadutil.h"

#include <exception>
#include <thread>

namespace cutl
{

static constexpr unsigned int MIN_THREAD_NUM = 1;

threadpool::threadpool(const std::string& name, uint32_t max_task_size)
  : name_(name)
  , is_running_(false)
  , config_{ 0, 0, 0, max_task_size, reject_policy::synchronous }
  , worker_count_(0)
  , worker_index_(0)
  , shutdown_now_(false)
{
    uint32_t thread_num = std::thread::hardware_concurrency();
    if (thread_num == 0)
    {
        thread_num = MIN_THREAD_NUM;
    }
    // Default: core = hardware threads, max = 2x, idle extra threads exit after 1 minute.
    config_.core_threads = thread_num;
    config_.max_threads = thread_num * 2;
    config_.keep_alive = 60000;
    CUTL_INFO("name: " << name_ << " core_threads: " << config_.core_threads
                       << " max_threads: " << config_.max_threads
                       << " keep_alive: " << config_.keep_alive
                       << " queue_capacity: " << config_.queue_capacity
                       << " policy: " << static_cast<uint32_t>(config_.policy));
}

threadpool::threadpool(const threadpool_config& config)
  : threadpool("threadpool", config)
{
}

threadpool::threadpool(const std::string& name, const threadpool_config& config)
  : name_(name)
  , is_running_(false)
  , config_(config)
  , worker_count_(0)
  , worker_index_(0)
  , shutdown_now_(false)
{
    CUTL_INFO("name: " << name_ << " core_threads: " << config_.core_threads
                       << " max_threads: " << config_.max_threads
                       << " keep_alive: " << config_.keep_alive
                       << " queue_capacity: " << config_.queue_capacity
                       << " policy: " << static_cast<uint32_t>(config_.policy));
    // A config-built pool accepts tasks immediately. An invalid config stays stopped.
    if (validate_config())
    {
        is_running_.store(true);
    }
}

threadpool::~threadpool()
{
    shutdown(false);
}

bool threadpool::validate_config() const
{
    if (config_.max_threads == 0)
    {
        CUTL_ERROR("Threadpool " << name_ << " max_threads must be >= 1");
        return false;
    }
    if (config_.core_threads > config_.max_threads)
    {
        CUTL_ERROR("Threadpool " << name_ << " core_threads must be <= max_threads");
        return false;
    }
    return true;
}

void threadpool::start(uint32_t thread_num)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_running_.load())
    {
        CUTL_WARN("Threadpool " << name_ << " is already running");
        return;
    }

    if (thread_num > 0)
    {
        config_.core_threads = thread_num;
        config_.max_threads = thread_num;
        CUTL_INFO("Start with core_threads: " << thread_num << " max_threads: " << thread_num);
    }

    shutdown_now_ = false;
    is_running_.store(true);
    while (worker_count_ < config_.core_threads && spawn_worker_unlocked(Task()))
    {
    }
    CUTL_INFO("Threadpool " << name_ << " started");
}

// clear_in_destroctor: drop queued work and return. The destructor waits for
// threads that are still inside a task, so stop() itself does not block on them.
void threadpool::stop(bool clear_in_destroctor)
{
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " << name_ << " is already stopped");
        return;
    }

    if (!clear_in_destroctor)
    {
        shutdown(false);
        CUTL_INFO("Threadpool " << name_ << " stopped");
        return;
    }

    std::deque<Task> dropped;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        mark_stopped_unlocked(true, &dropped);
    }
    dropped.clear();
    CUTL_INFO("Threadpool " << name_ << " stopped");
}

bool threadpool::add_task(const Task& task)
{
    return add_waiting(task, nullptr);
}

bool threadpool::add_task(const Task& task, const Duration& timeout)
{
    return add_waiting(task, &timeout);
}

bool threadpool::add_waiting(const Task& task, const Duration* timeout)
{
    if (!task)
    {
        CUTL_WARN("Threadpool " << name_ << " task is nullptr");
        return false;
    }
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " << name_ << " is already stopped");
        return false;
    }
    return enqueue_task(Task(task), timeout);
}

bool threadpool::execute(Task task)
{
    if (!task)
    {
        CUTL_WARN("Threadpool " << name_ << " task is nullptr");
        return false;
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " << name_ << " is already stopped");
        return false;
    }

    const place_result placed = place_task_unlocked(task);
    if (placed == place_result::full)
    {
        return reject_unlocked(lock, std::move(task));
    }
    return placed == place_result::accepted;
}

bool threadpool::shutdown(bool shutdown_now)
{
    std::deque<Task> dropped;
    std::unique_lock<std::mutex> lock(mutex_);
    const bool initiated = mark_stopped_unlocked(shutdown_now, &dropped);

    // Destroy dropped tasks outside the lock. Their destructors might call back in.
    lock.unlock();
    dropped.clear();
    lock.lock();

    done_cv_.wait(lock, [this]() { return worker_count_ == 0 && queue_.empty(); });
    return initiated;
}

size_t threadpool::thread_count() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return worker_count_;
}

size_t threadpool::queue_size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

bool threadpool::enqueue_task(Task task, const Duration* timeout)
{
    std::unique_lock<std::mutex> lock(mutex_);
    const auto deadline = timeout ? std::chrono::steady_clock::now() + *timeout
                                  : std::chrono::steady_clock::time_point{};

    for (;;)
    {
        const place_result placed = place_task_unlocked(task);
        if (placed == place_result::accepted)
        {
            return true;
        }
        if (placed == place_result::closed)
        {
            CUTL_WARN("Threadpool " << name_ << " is already stopped");
            return false;
        }
        if (placed == place_result::failed)
        {
            return false;
        }

        // Full: wait until a slot opens, the pool stops, or a worker exits.
        auto can_place = [this]()
        {
            return !is_running_.load() || queue_.size() < config_.queue_capacity ||
                   worker_count_ < config_.max_threads;
        };
        if (!timeout)
        {
            not_full_cv_.wait(lock, can_place);
            continue;
        }
        if (!not_full_cv_.wait_until(lock, deadline, can_place))
        {
            CUTL_WARN("Threadpool " << name_ << " post_task_for timeout");
            return false;
        }
    }
}

// Caller holds mutex_. task is moved-from only when the result is accepted or failed.
threadpool::place_result threadpool::place_task_unlocked(Task& task)
{
    if (!is_running_.load())
    {
        return place_result::closed;
    }

    // 1. Below the core size: run this task on a new thread.
    if (worker_count_ < config_.core_threads)
    {
        return spawn_worker_unlocked(std::move(task)) ? place_result::accepted
                                                       : place_result::failed;
    }

    // 2. Queue has room. If nobody is consuming yet, start one worker.
    if (queue_.size() < config_.queue_capacity)
    {
        queue_.emplace_back(std::move(task));
        work_cv_.notify_one();
        if (worker_count_ == 0)
        {
            spawn_worker_unlocked(Task());
        }
        return place_result::accepted;
    }

    // 3. Queue is full, but we may still grow up to max_threads.
    if (worker_count_ < config_.max_threads)
    {
        return spawn_worker_unlocked(std::move(task)) ? place_result::accepted
                                                       : place_result::failed;
    }

    // 4. Saturated.
    return place_result::full;
}

bool threadpool::mark_stopped_unlocked(bool drop_queue, std::deque<Task>* dropped)
{
    if (!is_running_.load())
    {
        return false;
    }

    is_running_.store(false);
    if (drop_queue)
    {
        shutdown_now_ = true;
        dropped->swap(queue_);
    }
    else if (!queue_.empty() && worker_count_ == 0 && !spawn_worker_unlocked(Task()))
    {
        // Nobody left to run the queue, and a new worker could not be started.
        dropped->swap(queue_);
    }
    work_cv_.notify_all();
    not_full_cv_.notify_all();
    return true;
}

bool threadpool::spawn_worker_unlocked(Task first_task)
{
    try
    {
        // detach() drops the C++ handle. The OS thread runs until thread_main()
        // returns. worker_count_ is what shutdown() waits on.
        std::thread(
          [this, first_task]() mutable
          {
              thread_main(std::move(first_task));
          })
          .detach();
    }
    catch (...)
    {
        CUTL_ERROR("Threadpool " << name_ << " failed to create worker");
        return false;
    }
    ++worker_count_;
    return true;
}

void threadpool::thread_main(Task first_task)
{
    set_current_thread_name(worker_name(worker_index_.fetch_add(1)));
    {
        // Take the lock once so the creator finishes spawn_worker_unlocked()
        // before this thread runs a task that might call back into the pool.
        std::lock_guard<std::mutex> lock(mutex_);
    }
    if (first_task)
    {
        invoke(first_task);
    }
    for (;;)
    {
        Task task;
        if (!wait_and_pop(task))
        {
            return;
        }
        invoke(task);
    }
}

bool threadpool::wait_and_pop(Task& out)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (;;)
    {
        if (shutdown_now_)
        {
            retire_unlocked();
            return false;
        }
        if (!queue_.empty())
        {
            out = std::move(queue_.front());
            queue_.pop_front();
            not_full_cv_.notify_one();
            return true;
        }
        if (!is_running_.load())
        {
            retire_unlocked();
            return false;
        }

        // Core threads wait until there is work or the pool stops.
        // Threads above core_threads leave after keep_alive of idleness.
        if (worker_count_ <= config_.core_threads)
        {
            work_cv_.wait(lock);
            continue;
        }
        if (config_.keep_alive == 0)
        {
            retire_unlocked();
            return false;
        }
        const auto status =
          work_cv_.wait_for(lock, std::chrono::milliseconds(config_.keep_alive));
        const bool still_extra = worker_count_ > config_.core_threads;
        if (status == std::cv_status::timeout && queue_.empty() &&
            (shutdown_now_ || !is_running_.load() || still_extra))
        {
            retire_unlocked();
            return false;
        }
    }
}

void threadpool::retire_unlocked()
{
    --worker_count_;
    not_full_cv_.notify_one();
    if (worker_count_ == 0)
    {
        done_cv_.notify_all();
    }
}

bool threadpool::reject_unlocked(std::unique_lock<std::mutex>& lock, Task task)
{
    switch (config_.policy)
    {
        case reject_policy::synchronous:
            lock.unlock();
            invoke(task);
            return true;
        case reject_policy::discard_newest:
            lock.unlock();
            return false;
        case reject_policy::discard_oldest:
            break;
    }

    if (queue_.empty())
    {
        CUTL_WARN("Threadpool " << name_ << " has no queued task to discard");
        lock.unlock();
        return false;
    }
    Task dropped = std::move(queue_.front());
    queue_.pop_front();
    queue_.emplace_back(std::move(task));
    work_cv_.notify_one();
    lock.unlock();
    dropped = Task();
    return true;
}

std::string threadpool::worker_name(uint32_t index) const
{
    const std::string suffix = "_" + std::to_string(index);
    if (name_.size() + suffix.size() <= 15)
    {
        return name_ + suffix;
    }
    if (suffix.size() >= 15)
    {
        return suffix.substr(0, 15);
    }
    return name_.substr(0, 15 - suffix.size()) + suffix;
}

void threadpool::invoke(Task& task)
{
    try
    {
        task();
    }
    catch (const std::exception& e)
    {
        CUTL_ERROR("Threadpool " << name_ << " task threw: " << e.what());
    }
    catch (...)
    {
        CUTL_ERROR("Threadpool " << name_ << " task threw an unknown exception");
    }
}

} // namespace cutl
