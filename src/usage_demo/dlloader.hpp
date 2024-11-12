#pragma once

#include "common.hpp"
#include "dlloader.h"

void TestDlLoader()
{
    PrintTitle("TestDlLoader");

    // auto so_path =
    //   "/home/spencer/workspace/sdk_dev_guide/src/best_practice/unix/cpp_util/libutil.so";
    auto so_path =
      R"(D:\workspace\sdk_dev_guide\src\best_practice\vs_project_best_practice\Debug\cpp_util.dll)";
    cutl::dlloader soloader(so_path);

    typedef int (*MathFun)(int, int);

    auto add = (MathFun)soloader.get_symbol("add");
    int result = add(3, 2);
    std::cout << "3 + 2 = " << result << std::endl;

    auto sub = (MathFun)soloader.get_symbol("sub");
    result = sub(3, 2);
    std::cout << "3 - 2 = " << result << std::endl;
}