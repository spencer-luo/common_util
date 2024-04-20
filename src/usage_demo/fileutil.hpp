#pragma once

#include "common.hpp"
#include "common_util/fileutil.h"

static const std::string kBaseDir = "/Users/spencer/workspace/common_util/fileutil_test";
static const std::string kTargetDir = "/Users/spencer/workspace/common_util/fileutil_test_copy";

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
    // ./dir2/dir1
    auto dir21 = basedir.join("dir2/dir1");
    cutl::createdir(dir21, true);
    // ./dir2/dir1/file211.txt
    cutl::createfile(dir21.join("file211.txt"));
    // ./dir2/dir1/file212.txt
    cutl::createfile(dir21.join("test12.txt"));
    // ./dir2/dir1/file213.txt
    cutl::createfile(dir21.join("file213.data"));
    // ./dir2/file22.data
    auto file22 = basedir.join("dir2/file22.txt");
    cutl::createfile(file22);
    // ./dir2/file23.txt
    auto file23 = basedir.join("dir2/file23.data");
    cutl::createfile(file23);
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

void TestReadAndWriteText()
{
    PrintSubTitle("read/write text");

    auto basedir = cutl::path(kBaseDir);
    // ./file4.data
    std::string content = "this is test data for writetext.";
    std::cout << "write data:" << content << std::endl;
    auto file4 = basedir.join("file4.data");
    cutl::writetext(file4, content);
    std::cout << "read data:" << std::endl;
    auto text = cutl::readtext(file4);
    std::cout << text << std::endl;

    // // read big file
    // auto bigfile = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/file_content.txt");
    // auto bigtext = cutl::readtext(bigfile);
    // std::cout << "read big file: " << std::endl;
    // std::cout << bigtext << std::endl;
}

void TestCreateSymlink()
{
    PrintSubTitle("create symlink");

    auto basedir = cutl::path(kBaseDir);
    auto link4 = basedir.join("link4");
    cutl::createlink(basedir.join("file4.data"), link4);
    std::cout << "link4: " << link4 << ", realpath: " << link4.realpath() << std::endl;
}

void TestFilesizeAndDirsize()
{
    PrintSubTitle("filesize/dirsize");

    auto bigfile = cutl::path("/Users/spencer/Downloads/MicrosoftEdge-120.0.2210.133.pkg");
    auto filesize = cutl::filesize(bigfile);
    std::cout << "filesize: " << filesize << ", human readable: " << cutl::fmt_filesize(filesize) << std::endl;
    auto dirpath = cutl::path("/Users/spencer/Downloads/");
    auto dirsize = cutl::dirsize(dirpath);
    std::cout << "dirsize: " << dirsize << ", human readable: " << cutl::fmt_filesize(dirsize) << std::endl;
}

void PrintFileList(const cutl::filevec &fileList)
{
    for (size_t i = 0; i < fileList.size(); i++)
    {
        auto item = fileList[i];
        auto flag = cutl::filetype_flag(item.type);
        std::cout << "[" << flag << "]:";
        std::cout << item.filepath << std::endl;
    }
}

void TestListfile()
{
    PrintSubTitle("listfile");

    auto basedir = cutl::path(kBaseDir);

    std::cout << "List all files/dirs in current directory:" << std::endl;
    cutl::filevec fileList = cutl::list_files(basedir);
    PrintFileList(fileList);

    std::cout << "List all files in current directory:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::file);
    PrintFileList(fileList);

    std::cout << "List all files/dirs in current directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::all, true);
    PrintFileList(fileList);

    std::cout << "List all files in current directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::file, true);
    PrintFileList(fileList);

    std::cout << "List all dirs in current directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::directory, true);
    PrintFileList(fileList);
}

void TestFindfile()
{
    PrintSubTitle("findfile");

    auto basedir = cutl::path(kBaseDir);

    std::cout << "Find all files with 'file' in current directory:" << std::endl;
    cutl::filevec fileList = cutl::find_files(basedir, "file");
    PrintFileList(fileList);

    std::cout << "Find all files with 'file' by recursive:" << std::endl;
    fileList = cutl::find_files(basedir, "file", true);
    PrintFileList(fileList);

    std::cout << "Find all files with '.data' extension in current directory:" << std::endl;
    fileList = cutl::find_files_by_extension(basedir, ".data");
    PrintFileList(fileList);

    std::cout << "Find all files with '.data' extension by recursive::" << std::endl;
    fileList = cutl::find_files_by_extension(basedir, ".data", true);
    PrintFileList(fileList);
}

void TestCopyFileAndDir()
{
    PrintSubTitle("copy file/dir");

    auto basedir = cutl::path(kBaseDir);
    cutl::copyfile(basedir.join("file4.data"), basedir.join("file4_bak.data"), true);
    cutl::copyfile(basedir.join("link4"), basedir.join("link4_bak"), true);

    auto targetdir = cutl::path(kTargetDir);
    cutl::copydir(basedir, targetdir);
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    TestGetCwd();
    // TestCreateFileAndDir();
    // TestReadAndWriteText();
    // TestCreateSymlink();
    // TestFilesizeAndDirsize();
    // TestListfile();
    // TestFindfile();
    TestCopyFileAndDir();
    // TestRemoveFileAndDir();
}