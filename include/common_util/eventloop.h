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
 * @file eventloop.h
 * @brief event loop
 * @author Spencer
 * @date 2025-04-30
 */

#pragma once

#include "threadpool.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace cutl
{

// 头文件中隐藏TimerTask的定义(非对外接口数据类型)，具体实现在eventloop.cpp中
struct TimerTask;
using TimerTaskWPtr = std::weak_ptr<TimerTask>;
using TimerTaskPtr = std::shared_ptr<TimerTask>;
bool TimerTaskCompare(const TimerTaskPtr& a, const TimerTaskPtr& b);
// 简化类型声明
using EventloopDuration = std::chrono::steady_clock::duration;
using EventloopTimePoint = std::chrono::steady_clock::time_point;
using EventloopTask = std::function<void()>;

/**
 * Timer task handler. The caller can use this object to cancel timer task.
 */
class timer_task_handler
{
public:
    /**
     * @brief Construct a new timer task handler object
     *
     * @param timer_task the timer task
     */
    explicit timer_task_handler(const TimerTaskPtr& timer_task);
    /**
     * @brief Destroy the timer task handler object
     *
     */
    ~timer_task_handler() = default;
    /**
     * @brief Construct a new timer task handler object
     *
     * @param other the other object of timer task handler
     */
    timer_task_handler(timer_task_handler&& other) noexcept;
    /**
     * @brief Move assignment function
     *
     * @param other the other object of timer task handler
     *
     * @return timer_task_handler&
     */
    timer_task_handler& operator=(timer_task_handler&& other) noexcept;

    // 禁用默认构造函数
    timer_task_handler() = delete;
    // 不可以复制，禁用拷贝构造函数和赋值运算符
    timer_task_handler(const timer_task_handler&) = delete;
    timer_task_handler& operator=(const timer_task_handler&) = delete;

    /**
     * @brief Cancel the timer task.
     *
     */
    void cancel();

    /**
     * @brief Is the timer task valid?
     *
     * @return true
     * @return false
     */
    bool isvalid();

private:
    // 定时任务弱引用
    TimerTaskWPtr timer_task_;
};

/**
 * @brief The class of event loop
 *
 */
class eventloop
{
public:
    using TimerTaskVec = std::vector<TimerTaskPtr>;

    /**
     * @brief Construct a new eventloop object
     *
     * @param task_max_size the max size of task queue
     * @param timer_task_max_size the max size of timer task queue
     */
    eventloop(uint32_t task_max_size = 20, uint32_t timer_task_max_size = 10);
    /**
     * @brief Destroy the eventloop object
     *
     */
    virtual ~eventloop();

    // 不可以复制
    eventloop(const eventloop&) = delete;
    eventloop& operator=(const eventloop&) = delete;

public:
    /**
     * @brief Post ordinary task
     *
     * @param task task object
     * @return true
     * @return false
     */
    bool post_event(const EventloopTask& task);

    /**
     * @brief Post timer task
     *
     * @param name the name of timer task
     * @param func the callback function of timer task
     * @param period the period of timer task
     * @param repeat the repeat times of timer task, -1 means repeat forever
     * @return timer_task_handler
     */
    timer_task_handler post_timer_event(const std::string& name,
                                        const EventloopTask& func,
                                        const EventloopDuration& period,
                                        int64_t repeat = -1);

    /**
     * @brief 运行EventLoopBase，如果满足运行条件该接口会阻塞直到Stop被调用
     */

    /**
     * @brief Start to run the event loop.
     * @note If the running conditions are satisfied, this interface will block until Stop is
     * called.
     */
    virtual void start();

    /**
     * @brief Stop the event loop.
     *
     */
    virtual void stop();

    /**
     * @brief Is the event loop running or not?
     *
     * @return true
     * @return false
     */
    bool is_running() { return is_running_.load(); }

    /**
     * @brief Is the calling thread equals the event loop thread or not?
     *
     * @return true
     * @return false
     */
    bool is_loop_thread() const;

private:
    // 唤醒Loop线程
    void wakeup();
    // 等待超时或者被唤醒
    void wait_for_timeout_or_wakeup(std::chrono::microseconds timeout);
    // 执行单次循序的任务
    void loop_once(EventloopDuration default_timeout);

protected:
    // 处理任务
    virtual size_t handle_task();
    // 执行到点的定时任务
    virtual size_t handle_timer_task();
    // 开始事件循环
    void start_loop();
    // 添加定时任务
    timer_task_handler post_to_priorityqueue(const std::string& name,
                                             const EventloopTask& func,
                                             const EventloopDuration& period,
                                             int repeat);
    // 从定时任务队列中获取离当前时间最近的定时任务的到期间隔
    EventloopDuration get_next_run_time();

    // 获取所有到期的定时任务
    TimerTaskVec get_expired_timer_tasks(EventloopTimePoint now);

protected:
    // 事件循环是否运行中
    std::atomic<bool> is_running_;
    // 事件循环的线程id
    std::thread::id loop_thread_id_;
    // 普通任务 队列， 特点：单次执行，先进先出
    std::mutex task_mutex_;
    std::list<EventloopTask> task_queue_;
    uint32_t task_max_size_;
    // 定时任务 优先队列， 特点：循环执行，时间优先
    std::mutex timer_task_mutex_;
    std::priority_queue<TimerTaskPtr, TimerTaskVec, decltype(&TimerTaskCompare)> timer_task_queue_;
    uint32_t timer_task_max_size_;
    // 唤醒Loop线程的条件变量
    std::mutex cv_mutex_;
    std::condition_variable cv_wakeup_;
};

/**
 * @brief The class of single thread event loop
 *
 */
class singlethread_eventloop : public eventloop
{
public:
    singlethread_eventloop(const std::string thread_name,
                           uint32_t task_max_size = 20,
                           uint32_t timer_task_max_size = 10);

    ~singlethread_eventloop() = default;

public:
    /**
     * @brief Start to run the event loop.
     * @note If the running conditions are satisfied, this interface will block until Stop is
     * called.
     */
    void start();

    /**
     * @brief Stop the event loop.
     *
     */
    void stop();

private:
    std::thread loop_thread_;
    std::string thread_name_;
};

/**
 * @brief The class of multi thread event loop
 *
 */
class multithread_eventloop : public eventloop
{
public:
    /**
     * @brief Construct a new multithread eventloop object
     *
     * @param task_max_size
     * @param timer_task_max_size
     * @param thread_num
     */
    multithread_eventloop(uint32_t task_max_size = 20,
                          uint32_t timer_task_max_size = 10,
                          uint32_t thread_num = 0);
    /**
     * @brief Destroy the multithread eventloop object
     *
     */
    ~multithread_eventloop();

public:
    /**
     * @brief Start to run the event loop.
     * @note If the running conditions are satisfied, this interface will block until Stop is
     * called.
     */
    void start();

    /**
     * @brief Stop the event loop.
     *
     */
    void stop();

private:
    size_t handle_task();
    size_t handle_timer_task();

private:
    uint32_t thread_num_;
    threadpool thread_pool_;
};

} // namespace cutl