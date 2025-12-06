@mainpage

[![zread](https://img.shields.io/badge/Ask_Zread-_.svg?style=flat&color=00b0aa&labelColor=000000&logo=data%3Aimage%2Fsvg%2Bxml%3Bbase64%2CPHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQuOTYxNTYgMS42MDAxSDIuMjQxNTZDMS44ODgxIDEuNjAwMSAxLjYwMTU2IDEuODg2NjQgMS42MDE1NiAyLjI0MDFWNC45NjAxQzEuNjAxNTYgNS4zMTM1NiAxLjg4ODEgNS42MDAxIDIuMjQxNTYgNS42MDAxSDQuOTYxNTZDNS4zMTUwMiA1LjYwMDEgNS42MDE1NiA1LjMxMzU2IDUuNjAxNTYgNC45NjAxVjIuMjQwMUM1LjYwMTU2IDEuODg2NjQgNS4zMTUwMiAxLjYwMDEgNC45NjE1NiAxLjYwMDFaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00Ljk2MTU2IDEwLjM5OTlIMi4yNDE1NkMxLjg4ODEgMTAuMzk5OSAxLjYwMTU2IDEwLjY4NjQgMS42MDE1NiAxMS4wMzk5VjEzLjc1OTlDMS42MDE1NiAxNC4xMTM0IDEuODg4MSAxNC4zOTk5IDIuMjQxNTYgMTQuMzk5OUg0Ljk2MTU2QzUuMzE1MDIgMTQuMzk5OSA1LjYwMTU2IDE0LjExMzQgNS42MDE1NiAxMy43NTk5VjExLjAzOTlDNS42MDE1NiAxMC42ODY0IDUuMzE1MDIgMTAuMzk5OSA0Ljk2MTU2IDEwLjM5OTlaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik0xMy43NTg0IDEuNjAwMUgxMS4wMzg0QzEwLjY4NSAxLjYwMDEgMTAuMzk4NCAxLjg4NjY0IDEwLjM5ODQgMi4yNDAxVjQuOTYwMUMxMC4zOTg0IDUuMzEzNTYgMTAuNjg1IDUuNjAwMSAxMS4wMzg0IDUuNjAwMUgxMy43NTg0QzE0LjExMTkgNS42MDAxIDE0LjM5ODQgNS4zMTM1NiAxNC4zOTg0IDQuOTYwMVYyLjI0MDFDMTQuMzk4NCAxLjg4NjY0IDE0LjExMTkgMS42MDAxIDEzLjc1ODQgMS42MDAxWiIgZmlsbD0iI2ZmZiIvPgo8cGF0aCBkPSJNNCAxMkwxMiA0TDQgMTJaIiBmaWxsPSIjZmZmIi8%2BCjxwYXRoIGQ9Ik00IDEyTDEyIDQiIHN0cm9rZT0iI2ZmZiIgc3Ryb2tlLXdpZHRoPSIxLjUiIHN0cm9rZS1saW5lY2FwPSJyb3VuZCIvPgo8L3N2Zz4K&logoColor=ffffff)](https://zread.ai/spencer-luo/common_util)

# 1. English

## 1.1. Source code

| Region | Platform | Source Code Repository URL |
| --- | --- | --- |
| Mainland China | Gitee | [https://gitee.com/spencer_luo/common_util](https://gitee.com/spencer_luo/common_util) |
| International | Github | [https://github.com/spencer-luo/common_util](https://github.com/spencer-luo/common_util) |

## 1.2. Overview

common_util is a general - purpose utility library for C++. It is lightweight and easy to operate.

common_util is implemented using modern C++ syntax (C++11), with "cutl" as the namespace. The naming convention of all its interfaces is consistent with that of the STL, and it can be used as an extensions of C++ STL. You can use common_util just like using the STL, For example:

**Usage demo:**

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

**Running result:**

```bash
fmt_uint: 00012
fmt_double: 3.1416
fmt_filesize: 32.8K
current time(UTC time): 2025-05-02 10:29:10.785
current time(local time): 2025-05-02 18:29:10.785
```

## 1.3. Library Features

- **Simple and lightweigh**: Based on basic C++ syntax and STL, it independent on any third-party libraries.
- **Multi-platform support**: Supports multiple platforms such as Linux/macOS/Windows, it can be compiled and run on multiple platforms.
- **Clean code**: The code style is clean, and the naming convention is consistent with the STL, you can use this library just like use the C++ standard library.
- **Comprehensive documentation**: All exported header files have detail comments, it has complete API reference and usage demos.
- **C++ version requirement**: This repository is implemented using C++11, It is support the projects with C++11 or higher versions.

## 1.4. Module Description

| Functional Category | Header File | Main Functions |
| :------------ | :-------------- | :----------------------------------------------------------------------------------------------------- |
| File Operations | `filepath.h` | Operations on file paths, such as path concatenation and file type determination. |
| File Operations | `filetype.h` | Definition of file types. |
| File Operations | `fileutil.h` | Operations on the file system, such as file creation/deletion, file reading/writing, etc. |
| String Processing | `strfmt.h` | String formatting, such as time formatting, file size formatting, conversion of int to hexadecimal, etc. |
| String Processing | `strutil.h` | Utility functions for string operations, such as case conversion, string splitting, string concatenation, etc. |
| String Processing | `verutil.h` | Version number handling, such as parsing version number substrings from strings and version number comparison. |
| Date and Time | `datetime.h` | A simple date and time class based on the system clock. |
| Date and Time | `timecount.h` | A timer for measuring the running time of functions. |
| Date and Time | `timer.h` | Timers, supporting single - task timers (delayed execution) and repeating - task timers (periodic execution). |
| Date and Time | `timeutil.h` | Utility functions for time processing, such as time unit conversion and obtaining timestamps. |
| Concurrent Programming | `threadpool.h` | Thread pool, a lightweight and simple implementation of a thread pool. |
| Concurrent Programming | `threadutil.h` | Utility functions related to threads, such as setting thread names and obtaining thread IDs. |
| Concurrent Programming | `eventloop.h` | Event loop, supporting normal tasks and timed tasks (timed tasks support specifying the number of executions and cancellation). Task execution comes in two versions: single - thread (`eventloop`) and multi - thread (`multithread_eventloop`). |
| System Utilities | `sysutil.h` | System utility functions, such as system calls, obtaining CPU architecture/endianness, etc. |
| System Utilities | `dlloader.h` | Dynamic loader for dynamic libraries (shared libraries). |
| Common Algorithms | `algoutil.h` | Supplementary to `<algorithm>`, providing some commonly used algorithm functions, such as those not available in C++11 but added in later versions. |
| Common Algorithms | `lrucache.h` | High - performance LRU algorithm template class with an average time complexity of `O(1)` for both `get` and `put`. |
| Common Algorithms | `hash.h` | Provides common hash function algorithms |
| Common Algorithms | `bitmap.h` | An efficient Bitmap data structure class, and provides multiple variant subtypes: `dynamic_bitmap`, `roaring_bitmap`, etc. |
| Common Algorithms | `bloomfilter.h` | Bloom filter algorithm |
| Common Algorithms | `hyperloglog.h` | HyperLogLog algorithm |
| Logging and Printing | `print.h` | Helper functions for console printing, including printing arrays, vectors, maps of basic types, and printing strings with colors. |
| Type Wrappers | `color.h` | Encapsulation class for ARGB color values. |
| Auxiliary Programming | `singleton.h` | Template class and macro definitions for the singleton pattern. |
| Auxiliary Programming | `statemachine.h` |  State machine, which can pre-define the state transition roadmap and perform checks during state transitions. |
| Others | `logtype.h` | Type definitions related to log records. |
| Others | `config.h` | Provides library initialization functions, and functions to obtain the library's version and name. |
| Others | `version.h` | Definition of the version number of the `common_util` library. |
| Others | `common_util.h` | The import header file for `common_util`, including all other header files. |

## 1.5. Testing Platforms

This library has been tested and verified on the following platforms:

| Operating System Platform and Version | C++ Compiler and Version | Testing Status | Remarks |
| ------------------------------------- | ------------------------ | -------------- | ------- |
| Windows 10                            | Visual Studio 2015       | tested       |         |
| Ubuntu 20.04.4                        | GNU GCC 9.4.0            | tested       |         |
| macOS xxx                             | Clang version 15.0.0     | tested       |         |
| Android                               |                          | To be tested |         |
| iOS                                   |                          | To be tested |         |
| HarmonyOS                             |                          | To be tested |         |

## 1.6. Usage of the Library

There are usage examples of each module in src/usage_demo.

```bash
common.hpp      # Common header file for the Demo
config.hpp      # Initialization configuration
datetime.hpp    # Usage of the datetime class
dlloader.hpp    # Usage of the dynamic library loader
eventloop.hpp   # Usage of the eventloop class
filepath.hpp    # Usage of the filepath class
fileutil.hpp    # Operations related to the file system
lrucache.hpp    # Usage of the LRU cache algorithm class
main.cpp        # Main function of the Demo
print.hpp       # Operations related to printing
singleton.hpp   # Usage of macro definitions related to the singleton pattern
strfmt.hpp      # Usage of String formatting functions
strutil.hpp     # Operations related to strings
sysutil.hpp     # Operations related to system calls
threadpool.hpp  # Usage of the threadpool class
threadutil.hpp  # Operations related to threads
timecount.hpp   # time counter of the function
timer.hpp       # Usage of the timer class
timeutil.hpp    # Operations related to time
verutil.hpp     # Operations related to version numbers
```

For more usage documentation, refer to:

```bash
docs/quick_start/Chinese/01.preparation.md     # environment preparation
docs/quick_start/Chinese/02.build_library.md   # Building and running
docs/quick_start/Chinese/03.getting_start.md   # Quick guide
```

## 1.7. Related Documents


- off-line: [docs/reference/html/index.html](./docs/reference/html/index.html)
- on line : [http://sunlogging.com/docs/common_util/](http://sunlogging.com/docs/common_util/)

## 1.8. Welcome to Collaborate

We welcome all developers who are interested in this project to join us, whether you are an experienced senior developer or a newcomer just starting out, we look forward to your participation. You can get involved in the project in the following ways:

- **Code Contribution:** You can fork our code repository, conduct code development, and submit your code via a pull request.
- **Issue Feedback:** If you encounter any problems during use or have any suggestions, feel free to raise them in the issue section.
- **Documentation Writing:** We welcome you to write documentation for the project, helping other developers better understand and use our project.
- **Testing and Feedback:** You can assist us in code testing and provide valuable feedback.

If you have any questions or need help, please feel free to contact me through the following methods:

Email: [spencer.luo@foxmail.com]

Looking forward to your participation!

# 2. 中文

## 2.1. 源码仓库

| 地区 | 代码托管平台 | 源码仓库地址 |
| --- | --- | --- |
| 中国大陆 | Gitee | [https://gitee.com/spencer_luo/common_util](https://gitee.com/spencer_luo/common_util) |
| 国际站点 | Github | [https://github.com/spencer-luo/common_util](https://github.com/spencer-luo/common_util) |

## 2.2. 项目概述

common_util是C++的一个通用工具库，轻量级、操作简便。

common_util采用现代C++语法（C++11）实现，使用cutl作为命名空间，所有接口的命名方式与STL保持一致，可以作为STL库的一个补充。你可以像使用STL一样使用common_util，如：

**用法示例：**

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

**运行结果：**

```bash
fmt_uint: 00012
fmt_double: 3.1416
fmt_filesize: 32.8K
current time(UTC time): 2025-05-02 10:29:10.785
current time(local time): 2025-05-02 18:29:10.785
```

## 2.3. 设计特点

- **轻量极简**： 基于基础C++语法和STL，不依赖任何第三方库。
- **多平台支持**： 支持Linux/macOS/Windows等多个平台，一套代码多平台编译。
- **代码整洁**： 代码风格整洁，命名方式与STL保持一致，你可以像使用STL一样使用本库。
- **文档规范全面**： 所有对外接口的头文件都有详细的代码注释，且有完整的API文档和使用Demo，可供查阅。
- **C++版本要求**： 本仓库采用C++11实现，C++11及以上的项目均支持。

## 2.4. 模块说明

| 功能分类 | 头文件          | 主要功能                                                                                               |
| :------------ | :-------------- | :----------------------------------------------------------------------------------------------------- |
| 文件操作 | `filepath.h`    | 文件路径的操作，如路径拼接、文件类型判断等。                                                           |
| 文件操作 | `filetype.h`    | 文件类型的定义。                                                                                       |
| 文件操作 | `fileutil.h`    | 文件系统的操作，如：文件的创建/删除、文件的读/写等。                                                   |
| 字符串处理 | `strfmt.h`      | 字符串格式化，如时间格式化、文件大小格式化、int转十六进制等。                                          |
| 字符串处理 | `strutil.h`     | 字符串操作的工具函数，如大小写转换、字符串拆分、字符串拼接等。                                         |
| 字符串处理 | `verutil.h`     | 版本号处理，如：从字符串中解析版本号子串、版本号比较等。                                               |
| 时间日期 | `datetime.h`    | 基于系统时钟的简易的日期时间类。                                                                       |
| 时间日期 | `timecount.h`   | 函数运行的使用时间计时器。                                                                             |
| 时间日期 | `timer.h`       | 定时器，支持：单次任务的定时器(延迟执行)、重复任务的定时器(周期执行)。                                 |
| 时间日期 | `timeutil.h`    | 时间处理的工具函数，如时间单位的转换、时间戳的获取等。                                                 |
| 并发编程 | `threadpool.h`  | 线程池，轻量级简单版本的线程池实现。                                                                   |
| 并发编程 | `threadutil.h`  | 线程相关的工具函数，如设置线程名称、获取线程ID等。                                                     |
| 并发编程 | `eventloop.h`   | 事件循环，支持：普通任务、定时任务(定时任务支持指定次数和取消)，任务的执行分为单线程(`eventloop`)和多线程(`multithread_eventloop`)两个版本。 |
| 系统工具 | `sysutil.h`     | 系统工具函数，如系统调用、获取CPU的架构/大小端等。                                                     |
| 系统工具 | `dlloader.h`    | 动态库(共享库)的动态加载器。                                                                           |
| 常用算法 | `algoutil.h`    | `<algorithm>`的补充，提供一些常用的算法函数，如：C++11没有，但是后面版本已加入的算法函数。             |
| 常用算法 | `lrucache.h` | 高性能LRU算法模板类，`get`和`put`的平均时间复杂度都是`O(1)`。 |
| 常用算法 | `hash.h` | 提供常用的哈希函数算法 |
| 常用算法 | `bitmap.h` | 高效的位图(Bitmap)数据结构类，并提供多个变种的子类型：dynamic_bitmap、roaring_bitmap等。 |
| 常用算法 | `bloomfilter.h` | 布隆过滤器算法 |
| 常用算法 | `hyperloglog.h` | HyperLogLog算法 |
| 日志与打印 | `print.h`       | 控制台打印的辅助函数，包括：打印基础类型的数组、vector、map等，带颜色的字符串打印。                    |
| 类型包装器 | `color.h`       | ARGB颜色值的封装类。                                                                                   |
| 辅助编程 | `singleton.h`   | 单例模式的模板类和宏定义。                                                                             |
| 辅助编程 | `statemachine.h` | 状态机，可预定义状态转换路径，并在在状态转换时进行检查。 |
| 其他 | `logtype.h`     | Log记录相关的类型定义。                                                                                |
| 其他 | `config.h`      | 提供库的初始化函数，提供库的版本和名称获取函数。                                                       |
| 其他 | `version.h`     | common_util库的版本号定义。                                                                            |
| 其他 | `common_util.h` | common_util的导入头文件，包含所有其他头文件。                                                          |

## 2.5. 已验证的平台

本库在以下平台上测试验证过：

| 操作系统平台及版本 | C++编译器及版本    | 验证状态 | 备注 |
| ------------------ | ------------------ | -------- | ---- |
| Windows 10         | Visual Studio 2015 | 已验证   |      |
| Ubuntu 20.04.4     | GNU GCC 9.4.0      | 已验证   |      |
| macOS xxx          | Clang version 15.0.0| 已验证   |      |
| Android            |                    | 待验证   |      |
| iOS                |                    | 待验证   |      |
| HarmonyOS          |                    | 待验证   |      |

## 2.6. 库的用法

src/usage_demo 有各个模块的使用示例。

```bash
common.hpp      # Demo的公共头文件
config.hpp      # 初始化配置
datetime.hpp    # 日期时间类的用法
dlloader.hpp    # 显示加载动态库的用法
eventloop.hpp   # 事件循环的用法
filepath.hpp    # 文件路径类的用法
fileutil.hpp    # 文件系统相关的操作
lrucache.hpp    # LRU缓存算法类的用法
main.cpp        # Demo的主函数
print.hpp       # 打印相关的操作
singleton.hpp   # 单例模式相关的宏定义的用法
strfmt.hpp      # 字符串格式化
strutil.hpp     # 字符串相关的操作
sysutil.hpp     # 系统调用相关的操作
threadpool.hpp  # 线程池的用法
threadutil.hpp  # 线程相关的操作
timecount.hpp   # 函数计时器类
timer.hpp       # 定时器的用法
timeutil.hpp    # 时间相关的操作
verutil.hpp     # 版本号相关的操作
```

更多的使用说明文档，参考：

```bash
docs/quick_start/中文/01.preparation.md     # 软件环境准备
docs/quick_start/中文/02.build_library.md   # 构建和运行
docs/quick_start/中文/03.getting_start.md   # 快速指引
```

## 2.7. 相关文档

- 离线文档: [docs/reference/html/index.html](./docs/reference/html/index.html)
- 在线文档: [http://sunlogging.com/docs/common_util/](http://sunlogging.com/docs/common_util/)

## 2.8. 欢迎共建

我们欢迎所有对此项目感兴趣的开发者加入我们，无论您是经验丰富的资深开发者，还是刚刚入门的新手，我们都期待您的参与。您可以通过以下方式参与到项目中：

- **代码贡献：** 您可以 fork 我们的代码仓库，进行代码开发，并通过 pull request 提交您的代码。
- **问题反馈：** 如果您在使用过程中遇到任何问题或有任何建议，欢迎在 issue 区提出。
- **文档编写：** 我们欢迎您为项目编写文档，帮助其他开发者更好地理解和使用我们的项目。
- **测试与反馈：** 您可以帮助我们进行代码测试，并提供宝贵的反馈意见。

如果您有任何疑问或需要帮助，欢迎通过以下方式联系我：

邮箱：[spencer.luo@foxmail.com]

期待你的参与！
