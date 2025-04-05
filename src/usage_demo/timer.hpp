#include "common.hpp"
#include "timer.h"
#include <memory>

// 示例用法
void sayHello(std::string name)
{
    std::cout << "Hello " << name << "!" << std::endl;
}

void TestSetTimeout()
{
    PrintSubTitle("set_timeout");

    // callback function 1
    cutl::set_timeout(sayHello, 2000, "Spencer");
    // callback function 2
    auto name = "Alice";
    cutl::set_timeout(sayHello, 2010, name);
    // lambda object
    cutl::set_timeout([name]() mutable { std::cout << "Lambda after 3 seconds!\n"; }, 3000);

    // 主线程等待足够时间以确保异步任务完成
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
    std::cout << "Main thread after 4 seconds.name:" << name << std::endl;
}

static std::mutex sayhello_mtx_;
void SayHello()
{
    std::lock_guard<std::mutex> lock(sayhello_mtx_);
    auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp(cutl::timeunit::ms));
    auto threadId = std::this_thread::get_id();
    std::cerr << "[" << curTime << " " << threadId << "]";
    // std::this_thread::sleep_for(std::chrono::milliseconds(400));
    std::cout << " Hello World!" << std::endl;
}

void TestTimerCase_1()
{
    // 【Case1】 一般用例
    cutl::timer timer1("TimerTest-1", SayHello, std::chrono::seconds(1));
    timer1.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void TestTimerCase_2()
{
    // 【Case2】 任务的执行时间超过轮训的周期
    int count = 0;
    cutl::timer timer2(
      "TimerTest-2",
      [&count]()
      {
          count++;
          auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp(cutl::timeunit::ms));
          auto threadId = std::this_thread::get_id();
          std::cerr << "[" << curTime << " " << threadId << "]";
          std::cout << count << ": Hello World!" << std::endl;
          if (count % 2 == 0)
          {
              // count 为偶数时，睡眠 1.5s
              std::this_thread::sleep_for(std::chrono::milliseconds(1500));
          }
      },
      std::chrono::seconds(1));
    timer2.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
void TestTimerCase_3()
{
    // 【Case3】 停止定时器
    cutl::timer timer3("TimerTest-3", SayHello, std::chrono::seconds(2));
    timer3.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    timer3.stop();
    std::cout << "Main thread finished." << std::endl;
}

class TimerTestClass
{
public:
    TimerTestClass()
    {
        hello_timer_ = std::unique_ptr<cutl::timer>(new cutl::timer(
          "TimerTest-4", std::bind(&TimerTestClass::sayHello, this), std::chrono::seconds(1)));
    }

    ~TimerTestClass() { hello_timer_->stop(); }

    void start() { hello_timer_->start(); }

    void sayHello()
    {
        count_++;
        auto curTime = cutl::fmt_timestamp_ms(cutl::timestamp(cutl::timeunit::ms));
        auto threadId = std::this_thread::get_id();
        std::cerr << "[" << curTime << " " << threadId << "]--";
        std::cout << count_ << ": Hello World!" << std::endl;
        if (count_ == 3)
        {
            hello_timer_->stop();
        }
    }

private:
    std::unique_ptr<cutl::timer> hello_timer_;
    std::atomic_int count_{ 0 };
};

void TestTimerCase_4()
{
    // 【Case4】 在回调函数中停止定时器
    TimerTestClass obj;
    obj.start();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void TestTimerCase_5()
{
    // 【Case5】 调整系统时间
    cutl::timer timer1("TimerTest-5", SayHello, std::chrono::seconds(1));
    timer1.start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

void TestTimerCase_6()
{
    // 【Case6】 暂停再继续
    cutl::timer timer1("TimerTest-6", SayHello, std::chrono::seconds(1));
    timer1.start();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    timer1.stop();
    timer1.start();
    std::this_thread::sleep_for(std::chrono::seconds(4));
}

void TestTimerClass()
{
    PrintSubTitle("timer");

    // TestTimerCase_1();
    // TestTimerCase_2();
    // TestTimerCase_3();
    // // 在回调函数中停止定时器
    // TestTimerCase_4();
    // 调整系统时间
    TestTimerCase_5();
    // 暂停再继续
    // TestTimerCase_6();

    std::cout << "Main thread finished." << std::endl;
}

void TestTimer()
{
    PrintTitle("timer");

    // TestSetTimeout();
    TestTimerClass();
}