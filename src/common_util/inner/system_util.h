#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

namespace cutl
{

    // 系统调用函数
    bool call_system(const std::string &cmd);

    int call_system_v2(const std::string& cmd);

    // stdout / stderr 分别接到两个管道。process 在 Unix 上是子进程 pid，
    // 在 Windows 上是进程句柄；0 表示没有可回收的子进程。
    struct command_pipe
    {
        FILE* stdout_fp = nullptr;
        FILE* stderr_fp = nullptr;
        std::intptr_t process = 0;
    };

    // 失败时 stdout_fp 或 stderr_fp 为空，errno 保留创建失败的原因。
    command_pipe pipline_open(const std::string &cmd);

    // 关闭两个管道并回收子进程。
    // 正常退出时返回退出码；被信号终止时返回 128 + 信号编号；无法回收时返回 -1。
    int pipline_close(command_pipe &pipe);

} // namespace cutl
