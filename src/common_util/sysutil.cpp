/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file sysutil.cpp
 * @brief
 * @author spencer.luo
 * @date 2024-05-13
 */

 #include <map>
 #include <iostream>
 #include <strutil.h>
 #include <cctype>
 #include <cerrno>
 #include <cstdlib>
 #include <cstring>
 #include <thread>
 #include "sysutil.h"
 #include "inner/logger.h"
 #include "inner/system_util.h"
 #include "inner/filesystem.h"
 
 namespace cutl
 {
 
     // https://www.cnblogs.com/Forgenvueory/p/12757271.html
     // https://blog.csdn.net/n5/article/details/70143942
     // https://blog.csdn.net/qq_40340448/article/details/122117270
     os_platform platform_type()
     {
 #if defined(_WIN32)
         // #ifdef _WIN64
         //         std::cout << "64-bit Windows" << std::endl;
         // #else
         //         std::cout << "32-bit Windows" << std::endl;
         // #endif
         return os_platform::os_windows;
 #elif defined(__APPLE__) || defined(__MACH__)
         return os_platform::os_macos;
 #elif defined(__linux__)
         return os_platform::os_linux;
 #elif defined(__unix__)
         return os_platform::os_unix;
 #else
         return os_platform::os_unknown;
 #endif
     }
 
     std::string architecture()
     {
 #if defined(_WIN32)
         return cutl::getenv("PROCESSOR_ARCHITECTURE", "");
 #else
         static std::string arch;
         if (arch.empty())
         {
             std::string cmd = "uname -m";
             callcmd(cmd, arch);
             CUTL_DEBUG("cmd: " << cmd << ", result: " << arch);
         }
         return arch;
 #endif
     }
 
     std::string platform_name(os_platform type)
     {
         static std::map<os_platform, std::string> platform_map = {
             {os_platform::os_windows, "Windows"},
             {os_platform::os_macos, "macOS"},
             {os_platform::os_linux, "Linux"},
             {os_platform::os_unix, "Unix"},
             {os_platform::os_unknown, "Unknown"},
         };
 
         auto iter = platform_map.find(type);
         if (iter != platform_map.end())
         {
             return iter->second;
         }
 
         return "unknown";
     }
 
     std::string cpp_stl_version()
     {
         static std::map<long, std::string> version_map = {
             {199711L, "C++98"},
             {201103L, "C++11"},
             {201402L, "C++14"},
             {201703L, "C++17"},
             {202002L, "C++20"},
         };
 
         std::string stlVersion;
         auto iter = version_map.find(__cplusplus);
         if (iter != version_map.end())
         {
             stlVersion = iter->second;
         }
 
         return std::to_string(__cplusplus) + " (" + stlVersion + ")";
     }
 
     uint16_t program_bit()
     {
         auto byte = sizeof(void*);
         return byte * 8;
     }
 
     // https://blog.csdn.net/Frederick_Fung/article/details/115333125?utm_source=miniapp_weixin
     endian endian_type()
     {
         int a = 0x01;
         uint8_t* pBtye = (uint8_t*)(&a);
         if (*pBtye == 1)
         {
             return endian::little;
         }
         else
         {
             return endian::big;
         }
     }
 
     uint16_t byteswap(uint16_t value)
     {
         auto data = (uint8_t *)(&value);
         byteswap(data, 2);
         return value;
     }
 
     uint32_t byteswap(uint32_t value)
     {
         auto data = (uint8_t *)(&value);
         byteswap(data, 4);
         return value;
     }
 
     uint64_t byteswap(uint64_t value)
     {
         auto data = (uint8_t *)(&value);
         byteswap(data, 8);
         return value;
     }
 
     void byteswap(uint8_t *data, uint32_t size)
     {
         uint32_t n = size / 2;
         for (uint32_t i = 0; i < n; i++)
         {
             uint8_t temp = data[i];
             data[i] = data[size - i - 1];
             data[size - i - 1] = temp;
         }
     }
 
     bool system(const std::string &cmd)
     {
         return call_system(cmd);
     }
 
     int system2(const std::string& cmd)
     {
         return call_system_v2(cmd);
     }
 
     namespace
     {
 
     struct stream_text
     {
         std::string text;
         bool reached_eof = false;
         bool read_error = false;
         int error_no = 0;
     };
 
     bool has_visible_text(const std::string& text)
     {
         for (unsigned char ch : text)
         {
             if (!std::isspace(ch))
             {
                 return true;
             }
         }
         return false;
     }
 
     // 读到流结束或出错为止。最后一次 fgets 返回 NULL 时区分 feof / ferror。
     stream_text read_stream(FILE* fp)
     {
         stream_text got;
         char buffer[1024];
         while (true)
         {
             errno = 0;
             if (fgets(buffer, sizeof(buffer), fp) == nullptr)
             {
                 if (ferror(fp))
                 {
                     got.read_error = true;
                     got.error_no = errno;
                 }
                 else if (feof(fp))
                 {
                     got.reached_eof = true;
                 }
                 break;
             }
             got.text.append(buffer);
         }
         return got;
     }
 
     // stderr 在另一个线程里读完，避免子进程把 stderr 管道写满后卡住 stdout。
     struct command_session
     {
         command_pipe pipe{};
         std::thread stderr_reader;
         bool finished = false;
 
         ~command_session()
         {
             finish();
         }
 
         int finish()
         {
             if (finished)
             {
                 return 0;
             }
             if (stderr_reader.joinable())
             {
                 stderr_reader.join();
             }
             const int status = pipline_close(pipe);
             finished = true;
             return status;
         }
     };
 
     } // namespace
 
     bool callcmd(const std::string &cmd, std::string &result)
     {
         result.clear();
 
         // stderr_got 必须比 session 先声明。session 析构时会 join 读线程，
         // 线程里还持有 stderr_got 的引用。
         stream_text stderr_got;
         command_session session;
         session.pipe = pipline_open(cmd);
         if (session.pipe.stdout_fp == nullptr || session.pipe.stderr_fp == nullptr)
         {
             const int err = errno;
             CUTL_ERROR("pipline_open error for cmd: " << cmd << ", err: " << std::strerror(err));
             return false;
         }
 
         FILE* stderr_fp = session.pipe.stderr_fp;
         session.stderr_reader = std::thread([&stderr_got, stderr_fp]() {
             try
             {
                 stderr_got = read_stream(stderr_fp);
             }
             catch (...)
             {
                 stderr_got.read_error = true;
             }
         });
 
         const stream_text stdout_got = read_stream(session.pipe.stdout_fp);
         if (stdout_got.read_error)
         {
             CUTL_ERROR("fgets stdout error for cmd: " << cmd << ", err: "
                                                       << std::strerror(stdout_got.error_no));
         }
         else if (stdout_got.reached_eof)
         {
             if (has_visible_text(stdout_got.text))
             {
                 CUTL_DEBUG("fgets stdout EOF for cmd: " << cmd);
             }
             else
             {
                 CUTL_ERROR("fgets stdout EOF with no output for cmd: " << cmd);
             }
         }
         else
         {
             CUTL_ERROR("fgets stdout returned NULL without eof or error for cmd: " << cmd);
         }
 
         // 先收完 stderr 再关管道，保证失败日志里能看到完整的错误输出。
         const int close_ret = session.finish();
         const bool got_output = !stdout_got.read_error && has_visible_text(stdout_got.text);
         if (!got_output)
         {
             if (stderr_got.read_error)
             {
                 CUTL_ERROR("fgets stderr error for cmd: " << cmd << ", err: "
                                                           << std::strerror(stderr_got.error_no));
             }
             else if (has_visible_text(stderr_got.text))
             {
                 CUTL_ERROR("stderr for cmd: " << cmd << ", output: " << strip(stderr_got.text));
             }
             else
             {
                 CUTL_ERROR("stderr empty for cmd: " << cmd);
             }
             return false;
         }
 
         // 只要 stdout 有有效内容就视为成功。ota_engine -v 这类命令会打印出版本号，
         // 但进程以非零退出；退出码只记警告，不改变返回值。
         result = strip(stdout_got.text);
         CUTL_DEBUG("cmd: " << cmd << ", result: " << result);
         if (close_ret != 0)
         {
             CUTL_WARN("pipline_close returned " << close_ret << " for cmd: " << cmd <<
                       " (child exit status), stdout already captured.");
         }
         return true;
     }
 
     std::string getenv(const std::string &name, const std::string &default_value)
     {
         const char *text = std::getenv(name.c_str());
         if (text == nullptr)
         {
             CUTL_ERROR("variable [" << name << "] not set, fallback to " << default_value);
             return default_value;
         }
 
         return std::string(text);
     }
 
     std::string getcwd()
     {
         return current_program_dir();
     }
 
     std::string homedir()
     {
 #if defined(_WIN32)
         return cutl::getenv("USERPROFILE", "");
 #else
         return cutl::getenv("HOME", "");
 #endif
     }
 
 } // namespace