#pragma once

#include "common.hpp"
#include "common_util/fileutil.h"

void TestConstructor()
{
    PrintSubTitle("constructor");

    cutl::filepath path1("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    cutl::filepath path2("/Users/spencer/workspace/common_util/src/usage_demo//");
    cutl::filepath path3(R"(/Users/spencer/workspace/common_util\src\usage_demo\)");
    cutl::filepath path4 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    auto path5 = path4;
    std::cout << "path1: " << path1 << std::endl;
    std::cout << "path2: " << path2 << std::endl;
    std::cout << "path3: " << path3 << std::endl;
    std::cout << "path4: " << path4 << std::endl;
    std::cout << "path5: " << path5 << std::endl;
}

void TestJoin()
{
    PrintSubTitle("join");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo");
    auto path2 = path1.join("filepath.hpp");

    std::cout << "path1: " << path1 << std::endl;
    std::cout << "path2: " << path2 << std::endl;
}

void TestDirnameBasename()
{
    PrintSubTitle("dirname/basename");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    std::cout << "path1: " << path1 << std::endl;
    std::cout << "dirname: " << path1.dirname() << std::endl;
    std::cout << "basename: " << path1.basename() << std::endl;
    auto path2 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo");
    std::cout << "path2: " << path2 << std::endl;
    std::cout << "dirname: " << path2.dirname() << std::endl;
    std::cout << "basename: " << path2.basename() << std::endl;
    auto path3 = cutl::path("filepath.hpp");
    std::cout << "path3: " << path3 << std::endl;
    std::cout << "dirname: " << path3.dirname() << std::endl;
    std::cout << "basename: " << path3.basename() << std::endl;

    auto symlink_path = cutl::path("/Users/spencer/workspace/common_util/fileutil_test/symlink");
    std::cout << "symlink_path: " << symlink_path << std::endl;
    std::cout << "realpath: " << symlink_path.realpath() << std::endl;
}

void TestExtenstion()
{
    PrintSubTitle("extension");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    std::cout << "path1: " << path1 << ", extension: " << path1.extension() << std::endl;
    auto path2 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath");
    std::cout << "path2: " << path2 << ", extension: " << path2.extension() << std::endl;
}

void TestExist()
{
    PrintSubTitle("extension");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    std::cout << "path1: " << path1 << ", exists: " << path1.exists() << std::endl;
    auto path2 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath");
    std::cout << "path2: " << path2 << ", exists: " << path2.exists() << std::endl;
    auto path3 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo");
    std::cout << "path3: " << path3 << ", exists: " << path3.exists() << std::endl;
}

void TestPermission()
{
    PrintSubTitle("permission");

    auto path = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    std::cout << "path: " << path << ", readable: " << path.readable() << std::endl;
    std::cout << "path: " << path << ", writable: " << path.writable() << std::endl;
    std::cout << "path: " << path << ", executable: " << path.executable() << std::endl;
    auto path2 = cutl::path("/Users/spencer/workspace/common_util/script/build.sh");
    std::cout << "path2: " << path2 << ", executable: " << path2.executable() << std::endl;
}

void TestFilePath()
{
    PrintTitle("filepath");

    TestConstructor();
    TestJoin();
    TestDirnameBasename();
    TestExtenstion();
    TestExist();
    TestPermission();
}