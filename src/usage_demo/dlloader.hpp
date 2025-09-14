#pragma once

#include "common.hpp"
#include "common_util/dlloader.h"

void TestDlLoader()
{
    PrintTitle("TestDlLoader");

    // auto so_path =
    //   "/home/spencer/workspace/sdk_dev_guide/src/best_practice/unix/cpp_util/libutil.so";
    auto so_path =
      R"(D:\workspace\sdk_dev_guide\src\best_practice\vs_project_best_practice\Debug\cpp_util.dll)";
    cutl::dlloader soloader(so_path);

    typedef int (*MathFun)(int, int);

    // 获取add函数并调用
    auto add = (MathFun)soloader.get_symbol("add");
    int result = add(3, 2);
    std::cout << "3 + 2 = " << result << std::endl;
    // 获取sub函数并调用
    auto sub = (MathFun)soloader.get_symbol("sub");
    result = sub(3, 2);
    std::cout << "3 - 2 = " << result << std::endl;
}