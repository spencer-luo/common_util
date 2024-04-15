#pragma once

#include "common.hpp"
#include "common_util/fileutil.h"

void TestCreateFileAndDir()
{
    PrintSubTitle("create file/dir");

    std::string cwd = cutl::getcwd();
    std::cout << "cwd: " << cwd << std::endl;

    auto basepath = cutl::path(cwd);
    auto testdir = basepath.join("test_dir");
    std::cout << "testdir: " << testdir << std::endl;
    if (!cutl::createdir(testdir))
    {
        std::cout << "failed to create test_dir" << testdir << std::endl;
        return;
    }
    auto testfile = testdir.join("test_file.txt");

    if (!cutl::createfile(testfile))
    {
        std::cout << "failed to create test_file.txt" << testfile << std::endl;
        return;
    }
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    TestCreateFileAndDir();
}