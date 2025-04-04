#include "common.hpp"
#include "timer.h"

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

void process(int&& x)
{
    // 只能接受右值
}

void TestTimer()
{
    PrintTitle("timer");

    TestSetTimeout();

    // int a = 10;
    // process(20);           // 合法：20 是右值
    // process(a);            // 非法：a 是左值
    // process(std::move(a)); // 合法：std::move(a) 转换为右值
}