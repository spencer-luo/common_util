#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace cutl
{

class threadpool
{
public:
    using Task = std::function<void()>;

public:
    threadpool(const std::string& name, uint32_t max_task_size = 1024);
    ~threadpool();

    void start(uint32_t num_threads = 0);

    void stop(bool clear_in_destroctor = false);

    bool add_task(const Task& task);
    bool add_task_for(const Task& task, std::chrono::microseconds timeout);

    template<class F, class... Args>
    auto add_task_with_args_and_return(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
          std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(task_mutex_);
            if (!is_running_.load())
                throw std::runtime_error("enqueue on stopped ThreadPool");
            post_task([task]() { (*task)(); });
        }
        cv_consumer_.notify_one();
        return res;
    }

private:
    bool post_task(const Task& task);
    bool post_task_for(const Task& task, std::chrono::microseconds timeout);
    void call_one_task();
    void clear();

private:
    // 线程池的名称
    std::string name_;
    // 任务调度是否运行中
    std::atomic<bool> is_running_;
    // 线程池中的线程对象
    std::vector<std::thread> threads_;
    // 任务队列
    std::mutex task_mutex_;
    std::list<Task> task_queue_;
    uint32_t max_task_size_;
    std::condition_variable cv_producer_;
    std::condition_variable cv_consumer_;
};

} // namespace cutl
