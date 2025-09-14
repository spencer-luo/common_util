/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations.
 *
 * @file threadpool.h
 * @brief thread pool
 * @author Spencer
 * @date 2025-04-30
 */

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

/**
 * @brief The thread pool class
 *
 */
class threadpool
{
public:
    using Task = std::function<void()>;
    using Duration = std::chrono::steady_clock::duration;

public:
    /**
     * @brief Construct a new threadpool object
     *
     * @param name
     * @param max_task_size
     */
    threadpool(const std::string& name, uint32_t max_task_size = 1024);
    /**
     * @brief Destroy the threadpool object
     *
     */
    ~threadpool();

    /**
     * @brief Start the threadpool
     *
     * @param num_threads
     */
    void start(uint32_t num_threads = 0);

    /**
     * @brief Stop the threadpool
     *
     * @param clear_in_destroctor
     */
    void stop(bool clear_in_destroctor = false);

    /**
     * @brief Add a task to the threadpool
     *
     * @param task the task function
     * @return true
     * @return false
     */
    bool add_task(const Task& task);

    /**
     * @brief Add a task to the threadpool with timeout
     *
     * @param task the task function
     * @param timeout the timeout duration
     * @return true
     * @return false
     */
    bool add_task(const Task& task, const Duration& timeout);

    /**
     * @brief Add a task to the threadpool with args and return
     *
     * @param f
     * @param args
     * @return std::future<typename std::result_of<F(Args...)>::type>
     */
    template<class F, class... Args>
    auto add_task_with_args_and_return(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
          std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            auto res = add_task([task]() { (*task)(); });
            if (!res)
            {
                throw std::runtime_error("add task failure!");
            }
        }
        cv_consumer_.notify_one();
        return res;
    }

private:
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
