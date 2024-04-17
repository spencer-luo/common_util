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

    auto basedir = cutl::path("/Users/spencer/workspace/common_util/fileutil_test");
    cutl::createdir(basedir);
    // ./dir1
    cutl::createdir(basedir.join("dir1"));
    // ./file1.txt
    cutl::createfile(basedir.join("file1.txt"));
    // ./file2.txt
    cutl::createfile(basedir.join("file2.txt"));
    // ./dir2/dir1
    auto dir21 = basedir.join("dir2/dir1");
    cutl::createdir(dir21, true);
    // ./dir2/dir1/file211.txt
    cutl::createfile(dir21.join("file211.txt"));
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    TestGetCwd();
    TestCreateFileAndDir();
}