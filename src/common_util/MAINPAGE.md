@mainpage

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
