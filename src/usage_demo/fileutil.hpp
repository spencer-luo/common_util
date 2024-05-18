#pragma once

#include "common.hpp"
#include "fileutil.h"

static const std::string kBaseDir = "./fileutil_test";
static const std::string kTargetDir = "./fileutil_test_copy";

void TestGetCwd()
{
    PrintSubTitle("TestGetCwd");

    std::string cwd = cutl::getcwd();
    std::cout << "cwd: " << cwd << std::endl;
}

void TestCreateFileAndDir()
{
    PrintSubTitle("TestCreateFileAndDir");

    // basedir
    auto basedir = cutl::path(kBaseDir);
    cutl::createdir(basedir);
    // level 1
    // ./test0.txt
    cutl::createfile(basedir.join("test0.txt"));
    // ./dir1
    cutl::createdir(basedir.join("dir1"));
    // ./file3.txt
    cutl::createfile(basedir.join("file3.txt"));
    // level 2
    // ./dir2/dir1
    auto dir21 = basedir.join("dir2/dir1");
    cutl::createdir(dir21, true);
    // ./dir2/file22.data
    auto file22 = basedir.join("dir2/file22.txt");
    cutl::createfile(file22);
    // ./dir2/file23.txt
    auto file23 = basedir.join("dir2/file23.data");
    // level 3
    // ./dir2/dir1/file211.txt
    cutl::createfile(dir21.join("file211.txt"));
    // ./dir2/dir1/file212.txt
    cutl::createfile(dir21.join("test212.txt"));
    // ./dir2/dir1/file213.txt
    cutl::createfile(dir21.join("file213.data"));

    cutl::createfile(file23);
}

void TestRemoveFileAndDir()
{
    PrintSubTitle("TestRemoveFileAndDir");

    auto basedir = cutl::path(kBaseDir);

    // ./dir1
    cutl::removedir(basedir.join("dir1"));
    // ./file1.txt
    cutl::removefile(basedir.join("file3.txt"));
    // ./dir2
    cutl::removedir(basedir.join("dir2"), true);
    // ./
    cutl::removedir(basedir, true);
    std::cout << basedir << " removed" << std::endl;
    // fileutil_test_copy
    cutl::removedir(cutl::path(kTargetDir), true);
    std::cout << kTargetDir << " removed" << std::endl;
}

void TestReadAndWriteText()
{
    PrintSubTitle("TestReadAndWriteText");

    auto basedir = cutl::path(kBaseDir);
    // ./file4.data
    std::string content = "this is test data for writetext.";
    std::cout << "write data:" << content << std::endl;
    auto file4 = basedir.join("file4.data");
    cutl::writetext(file4, content);
    std::cout << "read data:" << std::endl;
    auto text = cutl::readtext(file4);
    std::cout << text << std::endl;

    // read big file
    auto bigfile = cutl::path("./src/common_util/inner/filesystem_win.cpp");
    auto bigtext = cutl::readtext(bigfile);
    std::cout << "read big file: " << std::endl;
    std::cout << bigtext << std::endl;
}

void TestCreateSymlink()
{
    PrintSubTitle("TestCreateSymlink");

    auto basedir = cutl::path(kBaseDir);
    auto link4 = basedir.join("link4");
    auto is_success = cutl::createlink(basedir.join("file4.data").abspath(), link4);
    if (is_success)
    {
        std::cout << "link4: " << link4 << ", realpath: " << link4.realpath() << std::endl;
        std::cout << "read data from linkpath: " << cutl::readtext(link4) << std::endl;
        std::cout << "read data from realpath: " << cutl::readtext(link4.realpath()) << std::endl;
    }
}

void TestFilesizeAndDirsize()
{
    PrintSubTitle("TestFilesizeAndDirsize");

    auto smallfile = cutl::path("./fileutil_test/file4.data");
    auto smallfile_size = cutl::filesize(smallfile);
    std::cout << smallfile << " smallfile size: " << smallfile_size << ", human readable: " << cutl::fmt_filesize(smallfile_size) << std::endl;

    // // for Unix only
    // auto bigfile = cutl::path("/Users/spencer/Downloads/MicrosoftEdge-120.0.2210.133.pkg");
    // auto filesize = cutl::filesize(bigfile);
    // std::cout << "filesize: " << filesize << ", human readable: " << cutl::fmt_filesize(filesize) << std::endl;

    // for Windows only
    auto shortcut = cutl::path(R"(C:\Users\Public\Desktop\CMake-gui.lnk)");
    auto shortcut_size = cutl::filesize(shortcut);
    std::cout << shortcut << " shortcut size: " << shortcut_size << ", human readable: " << cutl::fmt_filesize(shortcut_size) << std::endl;

    auto dirpath = cutl::path("./");
    auto dirsize = cutl::dirsize(dirpath);
    std::cout << dirpath << " dirsize: " << dirsize << ", human readable: " << cutl::fmt_filesize(dirsize) << std::endl;
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
    PrintSubTitle("TestListfile");

    auto basedir = cutl::path(kBaseDir);
    // auto basedir = cutl::path(R"(C:\Users\vboxuser\Desktop)");

    std::cout << "List all files/dirs in current directory:" << std::endl;
    cutl::filevec fileList = cutl::list_files(basedir);
    PrintFileList(fileList);

    std::cout << "List all files in current directory:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::file);
    PrintFileList(fileList);

    std::cout << "List all files/dirs in directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::all, true);
    PrintFileList(fileList);

    std::cout << "List all files in directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::file, true);
    PrintFileList(fileList);

    std::cout << "List all dirs in directory by recursive:" << std::endl;
    fileList = cutl::list_files(basedir, cutl::filetype::directory, true);
    PrintFileList(fileList);
}

void TestFindfile()
{
    PrintSubTitle("TestFindfile");

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
    PrintSubTitle("TestCopyFileAndDir");

    auto basedir = cutl::path(kBaseDir);
    cutl::copyfile(basedir.join("file4.data"), basedir.join("file4_bak.data"), true);
    auto linkpath = basedir.join("link4");
    if (linkpath.exists())
    {
        // windows下不会有此软链接文件，所以不用测试
        cutl::copyfile(linkpath, basedir.join("link4_bak"), true);
    }

    auto targetdir = cutl::path(kTargetDir);
    cutl::copydir(basedir, targetdir);
}

void TestFileUtil()
{
    PrintTitle("fileutil");

    // TestGetCwd();
    TestCreateFileAndDir();
    TestReadAndWriteText();
    TestCreateSymlink();
    TestFilesizeAndDirsize();
    TestListfile();
    TestFindfile();
    TestCopyFileAndDir();
    // TestRemoveFileAndDir();
}