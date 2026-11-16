/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
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
 * @file timer.h
 * @brief Periodic timer and one-shot timeout
 * @author Spencer
 * @date 2024-05-13
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

namespace cutl
{

/**
 * @brief Log an exception caught while starting a timer or running its task.
 *
 * An exception must not leave a std::thread entry function, or the process
 * calls std::terminate. what == nullptr means the exception was not a
 * std::exception.
 *
 * @param source name printed in the log, such as the timer name or "set_timeout"
 * @param what exception text from std::exception::what(), or nullptr
 */
void report_timer_task_exception(const char* source, const char* what);

/**
 * @brief Run a callable and report any exception it throws.
 *
 * @tparam Func callable type
 * @param source name passed to report_timer_task_exception()
 * @param func callable to run
 * @return true if func returned normally
 * @return false if func threw
 */
template<typename Func>
bool invoke_timer_task(const char* source, Func&& func)
{
    try
    {
        std::forward<Func>(func)();
        return true;
    }
    catch (const std::exception& e)
    {
        report_timer_task_exception(source, e.what());
    }
    catch (...)
    {
        report_timer_task_exception(source, nullptr);
    }
    return false;
}

/**
 * @brief Run a callable once after a delay, on a detached thread.
 *
 * The callable and its arguments are copied into the thread. Exceptions from
 * the callable are reported and do not terminate the process. The thread cannot
 * be cancelled.
 *
 * @tparam Callable callable type
 * @tparam Args argument types forwarded to the callable
 * @param f callable to run
 * @param delay_ms delay before the call, in milliseconds
 * @param args arguments forwarded to f
 */
template<typename Callable, typename... Args>
void set_timeout(Callable&& f, uint32_t delay_ms, Args&&... args)
{
    // Bind the callable and its arguments, then move the result into the thread.
    auto task = std::bind(std::forward<Callable>(f), std::forward<Args>(args)...);

    std::thread(
      [delay_ms, task]() mutable
      {
          invoke_timer_task("set_timeout",
                            [delay_ms, &task]()
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
                                task();
                            });
      })
      .detach();
}

/**
 * @brief Periodic timer that runs one task on its own thread.
 *
 * The first callback runs immediately. Later callbacks keep a fixed period on
 * std::chrono::steady_clock, so changing the system time does not move the
 * schedule. Time spent inside the callback counts toward the current period.
 * If the callback overruns the period, missed ticks are skipped and the next
 * tick waits one full period.
 *
 * running_ means "the loop should continue". stop(false) only requests exit.
 * The destructor and stop(true) join the worker unless the caller is that
 * worker. Do not destroy a timer inside its own callback.
 */
class timer
{
public:
    using Task = std::function<void()>;
    using Duration = std::chrono::microseconds; // us

    // No default constructor. Copy and assignment are disabled.
    timer() = delete;
    timer(const timer&) = delete;
    timer& operator=(const timer&) = delete;

    /**
     * @brief Construct a timer. The task does not run until start().
     *
     * @param name timer name, also used as the worker thread name
     * @param task callback for each tick. start() rejects an empty task.
     * @param interval period between tick start times. start() rejects a
     * non-positive period.
     */
    timer(const std::string& name, Task task, Duration interval);

    /**
     * @brief Stop the timer and join its worker thread.
     */
    ~timer();

    /**
     * @brief Start the worker thread.
     *
     * Does nothing if the timer is already running. start() called from the
     * timer callback is ignored. A worker left behind by stop(false) is joined
     * before the new thread is created.
     */
    void start();

    /**
     * @brief Ask the worker thread to exit.
     *
     * When wait_for_stop is true, join the worker if it is still running,
     * including a worker left by an earlier stop(false). A call made on the
     * worker thread only sets the stop flag and skips join.
     *
     * @param wait_for_stop whether the caller waits until the worker exits
     */
    void stop(bool wait_for_stop = true);

    /**
     * @brief Whether the loop is still requested to run.
     *
     * This becomes false as soon as stop() is requested, which can be earlier
     * than the worker actually exiting.
     *
     * @return true if the loop should continue
     * @return false if stop has been requested or start() did not run
     */
    bool is_running() const { return running_.load(); }

private:
    void run();

    /// @return true when the caller is the timer worker thread
    bool on_timer_thread() const;

    /// Loop should keep running. Does not mean the worker has exited.
    std::atomic_bool running_{ false };
    /// Written at the start of run() and cleared when run() returns.
    std::atomic<std::thread::id> worker_id_{};
    std::string name_;
    Task task_;
    std::chrono::steady_clock::time_point current_period_start_;
    Duration interval_;
    std::thread thread_;
    /// Guards starting and joining thread_. run() does not hold it while waiting.
    std::mutex lifecycle_mutex_;
    /// Mutex for cv_stopping_. Held only while the worker is waiting.
    std::mutex mutex_stopping_;
    std::condition_variable cv_stopping_;
};

} // namespace cutl
