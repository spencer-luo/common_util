/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations.
 *
 * @file threadpool.h
 * @brief Thread pool
 * @author Spencer
 * @date 2025-04-30
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>

namespace cutl
{

/**
 * @brief Policy used when execute() cannot accept a new task.
 *
 * synchronous: run the task in the caller thread
 * discard_newest: drop the new task
 * discard_oldest: drop the oldest queued task, then enqueue the new one
 */
enum class reject_policy : uint8_t
{
    synchronous = 0,    // run in the caller thread
    discard_newest = 1, // drop the incoming task
    discard_oldest = 2, // drop the oldest queued task, then enqueue the new one
};

/**
 * @brief Configuration for a Java-style (ThreadPoolExecutor) thread pool.
 *
 * Submit path:
 *   1. If worker count is below core_threads, create a thread to run the task.
 *   2. Else try to enqueue; if the pool has no workers yet, create one.
 *   3. If the queue is full and worker count is below max_threads, create a
 *      thread.
 *   4. Else apply policy.
 *
 * keep_alive only applies to threads above core_threads. Core threads stay
 * until shutdown. queue_capacity 0 means no buffering: the task is given to a
 * thread immediately or rejected.
 */
struct threadpool_config
{
    size_t core_threads;     // threads kept until shutdown
    size_t max_threads;      // upper bound on worker threads
    uint32_t keep_alive;     // idle lifetime of extra threads, in milliseconds
    size_t queue_capacity;   // bounded queue size; 0 means no buffering
    reject_policy policy;    // used when the pool cannot accept a new task
};

/**
 * @brief Thread pool with a bounded queue and optional dynamic workers.
 */
class threadpool
{
public:
    using Task = std::function<void()>;
    using Duration = std::chrono::steady_clock::duration;

public:
    /**
     * @brief Construct a legacy thread pool. Call start() before adding tasks.
     *
     * @param name pool name, also used as the worker thread name prefix
     * @param max_task_size bounded queue capacity; add_task() waits when full
     */
    threadpool(const std::string& name, uint32_t max_task_size = 1024);

    /**
     * @brief Construct a configurable thread pool. Tasks are accepted immediately.
     *
     * @param config core/max threads, keep-alive, queue capacity and reject policy
     */
    explicit threadpool(const threadpool_config& config);

    /**
     * @brief Construct a configurable thread pool with a name.
     *
     * @param name pool name, also used as the worker thread name prefix
     * @param config core/max threads, keep-alive, queue capacity and reject policy
     */
    threadpool(const std::string& name, const threadpool_config& config);

    /**
     * @brief Destroy the thread pool and wait for workers to exit.
     */
    ~threadpool();

    threadpool(const threadpool&) = delete;
    threadpool& operator=(const threadpool&) = delete;

    /**
     * @brief Start the thread pool.
     *
     * Legacy pools must call this before add_task(). Configurable pools are
     * already running; a second call is ignored. After stop()/shutdown(), this
     * can be used to restart the pool.
     *
     * @param num_threads worker count for a legacy pool; 0 means keep the
     *        current core/max settings (from construction or a previous start)
     */
    void start(uint32_t num_threads = 0);

    /**
     * @brief Stop the thread pool.
     *
     * @param clear_in_destroctor true: drop queued tasks and return without
     *        waiting for workers; the destructor waits for in-flight tasks.
     *        false: finish queued tasks, then wait for workers to exit.
     */
    void stop(bool clear_in_destroctor = false);

    /**
     * @brief Add a task to the thread pool.
     *
     * Waits while the queue is full. Returns false if the pool is not running
     * or has been stopped.
     *
     * @param task the task function
     * @return true if the task was accepted, false otherwise
     */
    bool add_task(const Task& task);

    /**
     * @brief Add a task to the thread pool, waiting up to timeout for queue space.
     *
     * @param task the task function
     * @param timeout how long to wait when the queue is full
     * @return true if the task was accepted, false on timeout or if stopped
     */
    bool add_task(const Task& task, const Duration& timeout);

    /**
     * @brief Add a task with arguments and get its result through a future.
     *
     * If the pool cannot accept the task, it runs in the caller thread so the
     * future still becomes ready.
     *
     * @param f callable to run
     * @param args arguments forwarded to f
     * @return std::future of the task result
     */
    template<class F, class... Args>
    auto add_task_with_args_and_return(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
          std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        if (!add_task([task]() { (*task)(); }))
        {
            (*task)();
        }
        return res;
    }

    /**
     * @brief Submit a fire-and-forget task. Rejection follows the configured policy.
     *
     * @param task the task function
     * @return true if the task was queued, assigned to a worker, or run in the
     *         caller thread; false if it was dropped or the pool is not running
     */
    bool execute(Task task);

    /**
     * @brief Submit a task and obtain its result (or exception) through a future.
     *
     * If execute() rejects the task, it runs in the caller thread so the future
     * still becomes ready.
     *
     * @param func callable to run
     * @param args arguments forwarded to func
     * @return std::future of the task result
     */
    template<class F, class... Args>
    auto submit(F&& func, Args&&... args)
      -> std::future<typename std::result_of<
        typename std::decay<F>::type(typename std::decay<Args>::type...)>::type>
    {
        using Result = typename std::result_of<
          typename std::decay<F>::type(typename std::decay<Args>::type...)>::type;

        auto task = std::make_shared<std::packaged_task<Result()>>(
          std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        std::future<Result> future = task->get_future();
        if (!execute([task]() { (*task)(); }))
        {
            (*task)();
        }
        return future;
    }

    /**
     * @brief Shut down the thread pool.
     *
     * The call that initiates shutdown returns true; repeated calls return false.
     * shutdown_now == false: stop accepting tasks, finish the queue, then wait.
     * shutdown_now == true: stop accepting tasks, drop the queue; in-flight
     * tasks finish, then workers exit. Do not call from a pool task, or the
     * current worker cannot exit.
     *
     * @param shutdown_now whether to drop queued tasks
     * @return true if this call initiated shutdown
     */
    bool shutdown(bool shutdown_now = false);

    /**
     * @brief Current number of worker threads.
     *
     * @return worker count
     */
    size_t thread_count() const;

    /**
     * @brief Current number of queued tasks.
     *
     * @return queue size
     */
    size_t queue_size() const;

private:
    // Result of trying to hand a task to the pool. Caller must hold mutex_.
    enum class place_result
    {
        accepted, // queued, or running on a new worker
        full,     // queue is full and worker count is already at max
        closed,   // pool is not running
        failed,   // a new worker could not be created
    };

    // Check core/max thread counts.
    bool validate_config() const;
    // Shared body of the two add_task() overloads. timeout == nullptr waits forever.
    bool add_waiting(const Task& task, const Duration* timeout);
    // Wait until the task is placed, the pool closes, or timeout expires.
    bool enqueue_task(Task task, const Duration* timeout);
    // Place one task: new core thread, else queue, else new extra thread, else full.
    place_result place_task_unlocked(Task& task);
    // Stop accepting tasks. drop_queue also discards tasks not yet started.
    bool mark_stopped_unlocked(bool drop_queue, std::deque<Task>* dropped);
    // Create a detached worker. Caller must hold mutex_.
    bool spawn_worker_unlocked(Task first_task);
    // Worker thread entry: run first_task, then take tasks from the queue.
    void thread_main(Task first_task);
    // Pop a task, or retire this worker when it should exit.
    bool wait_and_pop(Task& out);
    // Decrement worker count; wake shutdown waiters if this was the last one.
    void retire_unlocked();
    // Apply the reject policy. lock is held on entry.
    bool reject_unlocked(std::unique_lock<std::mutex>& lock, Task task);
    // Build a worker thread name (truncated to 15 chars for Linux pthread).
    std::string worker_name(uint32_t index) const;
    // Run a task; log and swallow exceptions so the worker thread stays alive.
    void invoke(Task& task);

private:
    std::string name_;                      // pool name / worker thread name prefix
    std::atomic<bool> is_running_;          // whether the pool accepts new tasks
    threadpool_config config_;              // core/max threads, keep-alive, queue, policy

    mutable std::mutex mutex_;              // guards queue, worker count and shutdown_now_
    std::condition_variable work_cv_;       // workers wait here when the queue is empty
    std::condition_variable done_cv_;       // shutdown() waits until all workers have exited
    std::condition_variable not_full_cv_;   // add_task() waits here when the queue is full
    std::deque<Task> queue_;                // pending tasks
    size_t worker_count_;                   // live worker threads
    std::atomic<uint32_t> worker_index_;    // monotonically increasing worker name suffix
    bool shutdown_now_;                     // drop queued tasks; written only under mutex_
};

} // namespace cutl
