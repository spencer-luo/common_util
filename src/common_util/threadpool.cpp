#include "threadpool.h"
#include "algoutil.h"
#include "inner/logger.h"
#include "threadutil.h"

namespace cutl
{

static constexpr unsigned int MIN_THREAD_NUM = 1;
static constexpr unsigned int MAX_THREAD_NUM = 16;

threadpool::threadpool(const std::string& name, uint32_t max_task_size)
  : name_(name)
  , is_running_(false)
  , max_task_size_(max_task_size)
{
}

threadpool::~threadpool()
{
    if (is_running_.load())
    {
        stop();
    }

    if (!threads_.empty())
    {
        clear();
    }
}

void threadpool::start(uint32_t thread_num)
{
    if (is_running_.load())
    {
        CUTL_WARN("Threadpool " + name_ + " is already running");
        return;
    }

    if (thread_num == 0)
    {
        // 获取当前系统支持的并发线程数的估计值
        thread_num = std::thread::hardware_concurrency();
        CUTL_INFO("Threadpool " + name_ +
                  " set thread num to hardware_concurrency:" + std::to_string(thread_num));
    }
    else
    {
        thread_num = cutl::clamp(thread_num, MIN_THREAD_NUM, MAX_THREAD_NUM);
        CUTL_INFO("Threadpool " + name_ + " set thread num to " + std::to_string(thread_num));
    }

    is_running_.store(true);
    for (uint32_t i = 0; i < thread_num; i++)
    {
        auto thread = std::thread(
          [this]()
          {
              while (is_running_.load())
              {
                  call_one_task();
              }
          });
        cutl::set_current_thread_name(name_ + "_" + std::to_string(i));
        threads_.emplace_back(std::move(thread));
    }
    CUTL_INFO("Threadpool " + name_ + " started");
}

// clear_in_destroctor为true时，延迟到threadpool的析构函数中清理线程和任务队列
// 在有任务可能会长时间执行时使用，避免stop()调用常时间阻塞。
void threadpool::stop(bool clear_in_destroctor)
{
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " + name_ + " is already stopped");
        return;
    }

    is_running_.store(false);

    if (!clear_in_destroctor)
    {
        clear();
    }
    CUTL_INFO("Threadpool " + name_ + " stopped");
}

bool threadpool::add_task(const Task& task)
{
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " + name_ + " is already stopped");
        return false;
    }

    std::unique_lock<std::mutex> lock(task_mutex_);
    // 等待，直到 队列未满 或 线程池已停止
    cv_producer_.wait(
      lock, [this]() { return task_queue_.size() < max_task_size_ || !is_running_.load(); });
    task_queue_.emplace_back(std::move(task));
    lock.unlock();

    // 通知消费者消费
    cv_consumer_.notify_one();
    return true;
}

bool threadpool::add_task(const Task& task, const Duration& timeout)
{
    if (!is_running_.load())
    {
        CUTL_WARN("Threadpool " + name_ + " is already stopped");
        return false;
    }

    std::unique_lock<std::mutex> lock(task_mutex_);
    // 等待，直到 队列未满 或 线程池已停止
    std::chrono::steady_clock::time_point abs_timeout = std::chrono::steady_clock::now() + timeout;
    if (!cv_producer_.wait_until(
          lock,
          abs_timeout,
          [this]() { return task_queue_.size() < max_task_size_ || !is_running_.load(); }))
    {
        CUTL_WARN("Threadpool " + name_ + " post_task_for timeout");
        return false;
    }
    task_queue_.emplace_back(std::move(task));
    lock.unlock();

    // 通知消费者消费
    cv_consumer_.notify_one();
    return true;
}

// 从队列中取一个任务，并执行
void threadpool::call_one_task()
{
    std::unique_lock<std::mutex> lock(task_mutex_);
    if (task_queue_.empty())
    {
        lock.unlock();
        cv_producer_.notify_one();
        // CUTL_WARN("Threadpool " + name_ + " task_queue_ is empty");
        return;
    }

    // 等待，直到 队列不为空  或 线程池已停止
    cv_consumer_.wait(lock, [this]() { return !task_queue_.empty() || !is_running_.load(); });
    auto task = task_queue_.front();
    task_queue_.pop_front();
    lock.unlock();

    cv_producer_.notify_one();
    // 执行任务
    task();
}

void threadpool::clear()
{
    for (auto& th : threads_)
    {
        if (th.joinable())
        {
            th.join();
        }
    }
    threads_.clear();
    task_queue_.clear();
}

} // namespace cutl
