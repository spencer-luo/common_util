#pragma once

#include "common.hpp"
#include "fileutil.h"

void TestConstructor()
{
    PrintSubTitle("TestConstructor");

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
    PrintSubTitle("TestJoin");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo");
    auto path2 = path1.join("filepath.hpp");

    std::cout << "path1: " << path1 << std::endl;
    std::cout << "path2: " << path2 << std::endl;
}

void TestDirnameBasename()
{
    PrintSubTitle("TestDirnameBasename");

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
}

void TestRealpathAndAbspath()
{
    PrintSubTitle("TestRealpathAndAbspath");

    auto symlink_path = cutl::path("./fileutil_test/link4");
    std::cout << "symlink_path: " << symlink_path << std::endl;
    std::cout << "realpath: " << symlink_path.realpath() << std::endl;
    std::cout << "abspath: " << symlink_path.abspath() << std::endl;

    auto path1 = cutl::path("../common_util/fileutil_test/file4.data");
    std::cout << "abspath: " << path1.abspath()
              << ", exists: " << path1.exists() << std::endl;

    auto path2 = cutl::path("./fileutil_test/file4.data");
    std::cout << "abspath: " << path2.abspath()
              << ", exists: " << path2.exists() << std::endl;

    auto path3 = cutl::path("./fileutil_test/../../common_util/fileutil_test/file4.data");
    std::cout << "abspath: " << path3.abspath()
              << ", exists: " << path3.exists() << std::endl;

    auto path4 = cutl::path("./fileutil_test/../../common_util/fileutil_test/file_xx.data");
    std::cout << "abspath: " << path4.abspath()
              << ", exists: " << path4.exists() << std::endl;

    auto path5 = cutl::path("~/workspace/common_util/fileutil_test/file4.data");
    std::cout << "abspath: " << path5.abspath()
              << ", exists: " << path5.exists() << std::endl;
}

void TestFileType()
{
    PrintSubTitle("TestFileType");

    auto path1 = cutl::path("./fileutil_test/dir1");
    std::cout << "path1: " << path1 << ", type: " << path1.type() << ", " << filetype_flag(path1.type()) << std::endl;
    auto path2 = cutl::path("./fileutil_test/file3.txt");
    std::cout << "path2: " << path2 << ", type: " << path2.type() << ", " << filetype_flag(path2.type()) << std::endl;
    auto path3 = cutl::path("./fileutil_test/file4.data");
    std::cout << "path3: " << path3 << ", type: " << path3.type() << ", " << filetype_flag(path3.type()) << std::endl;
    auto path4 = cutl::path(R"(C:\Users\Public\Desktop\CMake-gui.lnk)");
    std::cout << "path4: " << path4 << ", type: " << path4.type() << ", " << filetype_flag(path4.type()) << std::endl;
    auto path5 = cutl::path(R"(C:\Users\vboxuser\Desktop\VisualStudio2015)");
    std::cout << "path5: " << path5 << ", type: " << path5.type() << ", " << filetype_flag(path5.type()) << std::endl;
    auto path6 = cutl::path(R"(C:\Users\Public\Desktop\VisualStudio2015.lnk)");
    std::cout << "path6: " << path6 << ", type: " << path6.type() << ", " << filetype_flag(path6.type()) << std::endl;
    auto path7 = cutl::path(R"(C:\Users\vboxuser\Desktop\VisualStudio2015.lnk)");
    std::cout << "path7: " << path7 << ", type: " << path6.type() << ", " << filetype_flag(path7.type()) << std::endl;
}

void TestExtenstion()
{
    PrintSubTitle("TestExtenstion");

    auto path1 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath.hpp");
    std::cout << "path1: " << path1 << ", extension: " << path1.extension() << std::endl;
    auto path2 = cutl::path("/Users/spencer/workspace/common_util/src/usage_demo/filepath");
    std::cout << "path2: " << path2 << ", extension: " << path2.extension() << std::endl;
}

void TestExist()
{
    PrintSubTitle("TestExist");

    auto path1 = cutl::path("./src/usage_demo/filepath.hpp");
    std::cout << "path1: " << path1 << ", exists: " << path1.exists() << std::endl;
    auto path2 = cutl::path("./src/usage_demo/filepath");
    std::cout << "path2: " << path2 << ", exists: " << path2.exists() << std::endl;
    auto path3 = cutl::path("./src/usage_demo");
    std::cout << "path3: " << path3 << ", exists: " << path3.exists() << std::endl;
}

void TestPermission()
{
    PrintSubTitle("TestPermission");

    auto path = cutl::path("./fileutil_test/file4.data");
    std::cout << "path: " << path << ", exists: " << path.exists() << std::endl;
    std::cout << "path: " << path << ", readable: " << path.readable() << std::endl;
    std::cout << "path: " << path << ", writable: " << path.writable() << std::endl;
    std::cout << "path: " << path << ", executable: " << path.executable() << std::endl;
    auto path2 = cutl::path("./script/build.sh");
    std::cout << "path2: " << path2 << ", executable: " << path2.executable() << std::endl;
}

void TestFilePath()
{
    PrintTitle("filepath");

    TestConstructor();
    TestJoin();
    TestDirnameBasename();
    TestRealpathAndAbspath();
    TestFileType();
    TestExtenstion();
    TestExist();
    TestPermission();
}