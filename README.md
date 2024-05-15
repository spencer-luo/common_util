# common_util

## Overview

common_util is a general-purpose C++ utility library, include the following functions:

- Time utilities
- Function timer
- Filepath utilities
- Filesystem utilities
- String utilities
- Data formatting
- Singleton pattern macro definition

common_util is implemented in modern C++ syntax (C++11 and above), using cutl as the namespace. The naming convention of all interfaces is consistent with the STL, and it can be used as an extensions of C++ standard library. You can use common_util just like using the STL, For example:

```C++
// Data formatting
std::cout << "fmt_uint: " << cutl::fmt_uint(12, 5) << std::endl;
std::cout << "fmt_double: " << cutl::fmt_double(3.141592653, 4) << std::endl;
std::cout << "fmt_filesize: " << cutl::fmt_filesize(33600) << std::endl;
// Get the current system time
auto now = cutl::datetime::now();
std::cout << "current time(UTC time): " << now.utctime() << std::endl;
std::cout << "current time(local time): " << now.format() << std::endl;
```

## Library Features

- **Simple and lightweigh**: Based on basic C++ syntax and STL, it independent on any third-party libraries.
- **Multi-platform support**: Supports multiple platforms such as Linux/macOS/Windows, it can be compiled and run on multiple platforms.
- **Clean code**: The code style is clean, and the naming convention is consistent with the STL, you can use this library just like use the C++ standard library.
- **Comprehensive documentation**: All exported header files have detail comments, it has complete API reference and usage demos.
- **C++ version requirement**: C++11 and above versions.

## Testing Platforms

This library has been tested and verified on the following platforms:

| Operating System Platform and Version | C++ Compiler and Version | Testing Status | Remarks |
| ------------------------------------- | ------------------------ | -------------- | ------- |
| Windows 10                            | Visual Studio 2015       | tested       |         |
| Ubuntu 20.04.4                        | GNU GCC 9.4.0            | tested       |         |
| macOS xxx                             | Clang version 15.0.0     | tested       |         |
| Android                               |                          | To be tested |         |
| iOS                                   |                          | To be tested |         |
| HarmonyOS                             |                          | To be tested |         |

## Related Documents

- API Reference: [docs/reference/html/index.html](./docs/reference/html/index.html)
- User Guide: [common_util wiki](https://gitee.com/spencer_luo/common_util/wikis/Home)

## 概述

common_util是C++的一个通用工具库，主要包含以下功能：
- 时间处理
- 函数计时器
- 路径处理
- 文件操作
- 字符串处理
- 数据格式化
- 单例模式宏定义

common_util采用现代C++语法（C++11及以上）实现，使用cutl作为命名空间，所有接口的命名方式与STL保持一致，可以作为STL库的一个补充。你可以像使用STL一样使用common_util，如：

```C++
// 数据格式化
std::cout << "fmt_uint: " << cutl::fmt_uint(12, 5) << std::endl;
std::cout << "fmt_double: " << cutl::fmt_double(3.141592653, 4) << std::endl;
std::cout << "fmt_filesize: " << cutl::fmt_filesize(33600) << std::endl;
// 获取系统当前时间
auto now = cutl::datetime::now();
std::cout << "current time(UTC time): " << now.utctime() << std::endl;
std::cout << "current time(local time): " << now.format() << std::endl;
```

## 设计特点

- **轻量极简**： 基于基础C++语法和STL，不依赖任何第三方库。
- **多平台支持**： 支持Linux/OS-X/Windows等多个平台，一套代码多平台编译。
- **代码整洁**： 代码风格整洁，命名方式与STL保持一致，你可以像使用STL一样使用本库。
- **文档规范全面**： 所有对外接口的头文件都有详细的代码注释，且有完整的API文档和使用Demo，可供查阅。
- **C++版本要求**： C++11及以上版本。

## 已验证的平台

本库在以下平台上测试验证过：

| 操作系统平台及版本 | C++编译器及版本    | 验证状态 | 备注 |
| ------------------ | ------------------ | -------- | ---- |
| Windows 10         | Visual Studio 2015 | 已验证   |      |
| Ubuntu 20.04.4     | GNU GCC 9.4.0      | 已验证   |      |
| macOS xxx          | Clang version 15.0.0| 已验证   |      |
| Android            |                    | 待验证   |      |
| iOS                |                    | 待验证   |      |
| HarmonyOS          |                    | 待验证   |      |

## 相关文档

- API文档: [docs/reference/html/index.html](./docs/reference/html/index.html)
- 使用指南: [common_util wiki](https://gitee.com/spencer_luo/common_util/wikis/Home)