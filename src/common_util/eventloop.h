#pragma once

#include <atomic>
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
 * 定时任务句柄，当句柄被析构时会自动取消对应的定时任务，使用方需要一直持有直到定时任务取消
 */
class timer_task_handler
{
public:
    explicit timer_task_handler(const TimerTaskPtr& timer_task);
    ~timer_task_handler() = default;
    // 可移动
    timer_task_handler(timer_task_handler&&) noexcept;
    timer_task_handler& operator=(timer_task_handler&&) noexcept;

    // 禁用默认构造函数
    timer_task_handler() = delete;
    // 不可以复制，禁用拷贝构造函数和赋值运算符
    timer_task_handler(const timer_task_handler&) = delete;
    timer_task_handler& operator=(const timer_task_handler&) = delete;

    /**
     * 取消定时任务
     */
    void cancel();

    /**
     * 定时任务是否有效。
     */
    bool isvalid();

private:
    // 定时任务弱引用
    TimerTaskWPtr timer_task_;
};

class eventloop
{
public:
    using TimerTaskVec = std::vector<TimerTaskPtr>;

    eventloop();
    ~eventloop();

    // 不可以复制
    eventloop(const eventloop&) = delete;
    eventloop& operator=(const eventloop&) = delete;

    /**
     * 转发任务到EventLoopBase的线程执行
     * @param task 任务
     */
    void post_event(const EventloopTask& task);

    /**
     * 增加定时任务
     * @param name 定时任务名称
     * @param func 任务
     * @param period 周期
     * @param repeat 重复次数，默认值-1表示无限次执行
     * @return 定时任务句柄
     */
    timer_task_handler post_timer_event(const std::string& name,
                                        const EventloopTask& func,
                                        const EventloopDuration& period,
                                        int64_t repeat = -1);

    /**
     * 运行EventLoopBase，如果满足运行条件该接口会阻塞直到Stop被调用
     * @return true表示运行成功，false表示运行失败
     */
    bool start();

    /**
     * 停止EventLoopBase
     */
    void stop();

    /**
     * @brief 是否正在运行
     *
     * @return true
     * @return false
     */
    bool is_running() { return is_running_.load(); }

    /**
     * 当前线程是否Loop线程
     * @return true表示是，false表示不是
     */
    bool is_loop_thread() const;

    // TODO: 获取普通任务的数量（只供测试使用！）
    size_t test_get_task_size();

private:
    // 唤醒Loop线程
    void wakeup() const;
    // 等待超时或者被唤醒
    void wait_for_timeout_or_wakeup(std::chrono::microseconds timeout);
    // 执行单次循序的任务
    void loop_once(EventloopDuration timeout);

private:
    // 处理任务
    size_t handle_task();
    // 执行到点的定时任务
    size_t handle_timer_task();
    // 添加定时任务
    timer_task_handler post_to_priorityqueue(const std::string& name,
                                             const EventloopTask& func,
                                             const EventloopDuration& period,
                                             int repeat);
    // 从定时任务队列中获取离当前时间最近的定时任务的到期间隔
    EventloopDuration get_next_run_time();

    // 获取所有到期的定时任务
    TimerTaskVec get_expired_timer_tasks(EventloopTimePoint now);

private:
    // 时间循环是否运行中
    std::atomic<bool> is_running_;
    // 事件循环的线程id
    std::thread::id loop_thread_id_;
    // 普通任务 队列， 特点：单次执行，先进先出
    std::mutex task_mutex_;
    std::list<EventloopTask> task_queue_;
    // 定时任务 优先队列， 特点：循环执行，时间优先
    std::mutex timer_task_mutex_;
    std::priority_queue<TimerTaskPtr, TimerTaskVec, decltype(&TimerTaskCompare)> timer_task_queue_;
    // TODO: 事件fd
    int event_fd_;
};

} // namespace cutl