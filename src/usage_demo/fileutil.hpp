#pragma once

#include "common.hpp"
#include "common_util/fileutil.h"

void TestGetCwd()
{
    PrintSubTitle("getcwd");

    std::string cwd = cutl::getcwd();
    std::cout << "cwd: " << cwd << std::endl;
}

void TestCreateFileAndDir()
{
    PrintSubTitle("create file/dir");

    auto basepath = cutl::path("/Users/spencer/workspace/common_util/dest");
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

    auto deepdir = basepath.join("test_dir2/test_dir3");
    if (!cutl::createdir(deepdir, true))
    {
        std::cout << "failed to create deep dir" << deepdir << std::endl;
        return;
    }
    auto testfile2 = deepdir.join("test_file2.txt");
    cutl::createfile(testfile2);
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    TestGetCwd();
    TestCreateFileAndDir();
}