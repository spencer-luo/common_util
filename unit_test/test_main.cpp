/**
 * @file test_main.cpp
 * @brief common_util 单元测试入口，初始化日志回调后再执行 gtest 用例。
 */

#include "common_util/config.h"
#include "common_util/logtype.h"
#include <gtest/gtest.h>
#include <iostream>
#include <mutex>
#include <string>

namespace
{

// 测试期间静音库内部的 info/warn 日志，仅打印 error 日志，避免污染测试输出。
void quiet_log_func(cutl::loglevel level, const std::string& msg)
{
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    if (level == cutl::loglevel::error_level)
    {
        std::cerr << "[cutl-error] " << msg << std::endl;
    }
}

} // namespace

int main(int argc, char** argv)
{
    cutl::library_init(quiet_log_func);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
