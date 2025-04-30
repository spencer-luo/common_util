@mainpage

# 1. English

## 1.1. Source code

| Region | Platform | Source Code Repository URL |
| --- | --- | --- |
| Mainland China | Gitee | [https://gitee.com/spencer_luo/common_util](https://gitee.com/spencer_luo/common_util) |
| International | Github | [https://github.com/spencer-luo/common_util](https://github.com/spencer-luo/common_util) |

## 1.2. Overview

common_util is a general-purpose C++ utility library, include the following functions:

- Time utilities
- Function timer
- Filepath utilities
- Filesystem utilities
- String utilities
- Data formatting
- Singleton pattern macro definition
- Dynamic library loader

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

## 1.3. Library Features

- **Simple and lightweigh**: Based on basic C++ syntax and STL, it independent on any third-party libraries.
- **Multi-platform support**: Supports multiple platforms such as Linux/macOS/Windows, it can be compiled and run on multiple platforms.
- **Clean code**: The code style is clean, and the naming convention is consistent with the STL, you can use this library just like use the C++ standard library.
- **Comprehensive documentation**: All exported header files have detail comments, it has complete API reference and usage demos.
- **C++ version requirement**: C++11 and above versions.

## 1.4. Testing Platforms

This library has been tested and verified on the following platforms:

| Operating System Platform and Version | C++ Compiler and Version | Testing Status | Remarks |
| ------------------------------------- | ------------------------ | -------------- | ------- |
| Windows 10                            | Visual Studio 2015       | tested       |         |
| Ubuntu 20.04.4                        | GNU GCC 9.4.0            | tested       |         |
| macOS xxx                             | Clang version 15.0.0     | tested       |         |
| Android                               |                          | To be tested |         |
| iOS                                   |                          | To be tested |         |
| HarmonyOS                             |                          | To be tested |         |

## 1.5. Related Documents

- off-line: [docs/reference/html/index.html](./docs/reference/html/index.html)
- on line : [http://sunlogging.com/docs/common_util/](http://sunlogging.com/docs/common_util/)

## 1.6. Welcome to Collaborate

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

## 2.2. 概述

common_util是C++的一个通用工具库，轻量级、操作简便。

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

## 2.3. 设计特点

- **轻量极简**： 基于基础C++语法和STL，不依赖任何第三方库。
- **多平台支持**： 支持Linux/OS-X/Windows等多个平台，一套代码多平台编译。
- **代码整洁**： 代码风格整洁，命名方式与STL保持一致，你可以像使用STL一样使用本库。
- **文档规范全面**： 所有对外接口的头文件都有详细的代码注释，且有完整的API文档和使用Demo，可供查阅。
- **C++版本要求**： C++11及以上版本。

## 2.4. 模块说明

| 头文件          | 主要功能                                                                                               |
| :-------------- | :----------------------------------------------------------------------------------------------------- |
| `algoutil.h`    | `<algorithm>`的补充，提供一些常用的算法函数，如：C++11没有，但是后面版本已加入的算法函数。             |
| `color.h`       | ARGB颜色值的封装类。                                                                                   |
| `common_util.h` | common_util的核心头文件，包含所有其他头文件。                                                          |
| `config.h`      | 提供库的初始化函数，提供库的版本和名称获取函数。                                                       |
| `datetime.h`    | 基于系统时钟的简易的日期时间类。                                                                       |
| `dlloader.h`    | 动态库(共享库)的动态加载器。                                                                           |
| `eventloop.h`   | 事件循环，支持：普通任务、定时任务(定时任务支持指定次数和取消)，任务的执行分为单线程和多线程两个版本。 |
| `filepath.h`    | 文件路径的操作，如路径拼接、文件类型判断等。                                                           |
| `filetype.h`    | 文件类型的定义。                                                                                       |
| `fileutil.h`    | 文件系统的操作，如：文件的创建/删除、文件的读/写等。                                                   |
| `logtype.h`     | Log记录相关的类型定义。                                                                                |
| `print.h`       | 控制台打印的辅助函数，包括：打印基础类型的数组、vector、map等，带颜色的字符串打印。                    |
| `singleton.h`   | 单例模式的模板类和宏定义。                                                                             |
| `strfmt.h`      | 字符串格式化，如时间格式化、文件大小格式化、int转十六进制等。                                          |
| `strutil.h`     | 字符串操作的工具函数，如大小写转换、字符串拆分、字符串拼接等。                                         |
| `sysutil.h`     | 系统工具函数，如系统调用、获取CPU的架构/大小端等。                                                     |
| `threadpool.h`  | 线程池，轻量级简单版本的线程池实现。                                                                   |
| `threadutil.h`  | 线程相关的工具函数，如设置线程名称、获取线程ID等。                                                     |
| `timecount.h`   | 函数运行的使用时间计时器。                                                                             |
| `timer.h`       | 定时器，支持：单次任务的定时器(延迟执行)、重复任务的定时器(周期执行)。                                 |
| `timeutil.h`    | 时间处理的工具函数，如时间单位的转换、时间戳的获取等。                                                 |
| `version.h`     | common_util库的版本号定义。                                                                            |
| `verutil.h`     | 版本号处理，如：从字符串中解析版本号子串、版本号比较等。                                               |

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

## 2.6. 相关文档

- 离线文档: [docs/reference/html/index.html](./docs/reference/html/index.html)
- 在线文档: [http://sunlogging.com/docs/common_util/](http://sunlogging.com/docs/common_util/)

## 2.7. 欢迎共建

我们欢迎所有对此项目感兴趣的开发者加入我们，无论您是经验丰富的资深开发者，还是刚刚入门的新手，我们都期待您的参与。您可以通过以下方式参与到项目中：

- **代码贡献：** 您可以 fork 我们的代码仓库，进行代码开发，并通过 pull request 提交您的代码。
- **问题反馈：** 如果您在使用过程中遇到任何问题或有任何建议，欢迎在 issue 区提出。
- **文档编写：** 我们欢迎您为项目编写文档，帮助其他开发者更好地理解和使用我们的项目。
- **测试与反馈：** 您可以帮助我们进行代码测试，并提供宝贵的反馈意见。

如果您有任何疑问或需要帮助，欢迎通过以下方式联系我：

邮箱：[spencer.luo@foxmail.com]

期待你的参与！
