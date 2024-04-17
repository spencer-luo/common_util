#pragma once

#include "common.hpp"
#include "common_util/fileutil.h"

static const std::string kBaseDir = "/Users/spencer/workspace/common_util/fileutil_test";

void TestGetCwd()
{
    PrintSubTitle("getcwd");

    std::string cwd = cutl::getcwd();
    std::cout << "cwd: " << cwd << std::endl;
}

void TestCreateFileAndDir()
{
    PrintSubTitle("create file/dir");

    auto basedir = cutl::path(kBaseDir);
    cutl::createdir(basedir);
    // ./dir1
    cutl::createdir(basedir.join("dir1"));
    // ./file3.txt
    cutl::createfile(basedir.join("file3.txt"));
    // ./file4.txt
    cutl::createfile(basedir.join("file4.txt"));
    // ./dir2/dir1
    auto dir21 = basedir.join("dir2/dir1");
    cutl::createdir(dir21, true);
    // ./dir2/dir1/file211.txt
    cutl::createfile(dir21.join("file211.txt"));
    // ./dir2/dir1/file212.txt
    cutl::createfile(dir21.join("file212.txt"));
    // ./dir2/file22.data
    auto file22 = basedir.join("dir2/file22.data");
    cutl::createfile(file22);
}

void TestRemoveFileAndDir()
{
    PrintSubTitle("remove file/dir");

    auto basedir = cutl::path(kBaseDir);

    // ./dir1
    cutl::removedir(basedir.join("dir1"));
    // ./file1.txt
    cutl::removefile(basedir.join("file3.txt"));
    // ./dir2
    cutl::removedir(basedir.join("dir2"), true);
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    TestGetCwd();
    // TestCreateFileAndDir();
    TestRemoveFileAndDir();
}