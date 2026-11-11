#if defined(_WIN32)
// do nothing
#else

#include "inner/logger.h"
#include "system_util.h"
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>

namespace cutl
{

std::string parse_exitcode(int exit_status)
{
    // Common exit status code explanations
    static const std::unordered_map<int, std::string> exitCodeMap = {
        { 0, "Command executed successfully" },
        { 1, "General error or command usage error" },
        { 2, "Command parameter error" },
        { 126, "Command not executable (insufficient permissions)" },
        { 127, "Command not found" },
    };

    auto it = exitCodeMap.find(exit_status);
    if (it != exitCodeMap.end())
    {
        return it->second;
    }
    else
    {
        return "Unknown exit status code";
    }
}

std::string parse_signal(int signal)
{
    // Common signal explanations
    static const std::unordered_map<int, std::string> signalMap = {
        { SIGTERM, "SIGTERM (termination request)" },
        { SIGKILL, "SIGKILL (force termination)" },
        { SIGSEGV, "SIGSEGV (segmentation fault)" },
        { SIGINT, "SIGINT (interrupt, usually Ctrl+C)" },
        { SIGABRT, "SIGABRT (program abnormal termination)" },
    };
    auto it = signalMap.find(signal);
    if (it != signalMap.end())
    {
        return it->second;
    }
    else
    {
        return "Unknown signal";
    }
}

bool call_system(const std::string& cmd)
{
    if (cmd.empty())
    {
        CUTL_ERROR("cmd is empty!");
        return false;
    }

    pid_t status;
    status = std::system(cmd.c_str());

    // The child process was not created.
    if (-1 == status)
    {
        CUTL_ERROR(
          "Child process could not be created, or its status could not be retrieved. err msg:" <<
          std::strerror(errno));
        return false;
    }

    // (status & 0x7f ) == 0
    // Child process exited normally (exec completed)
    if (WIFEXITED(status))
    {
        // ((status) & 0xff00) >> 8
        int exit_code = WEXITSTATUS(status);
        if (exit_code == 0)
        {
            return true;
        }
        else
        {
            CUTL_WARN("The child process exited normally, exit status:" <<
                      exit_code << ", exit msg: " << parse_exitcode(exit_code));
            return false;
        }
    }

    // Child process terminated by signal
    // ((signed char) (((status) & 0x7f) + 1) >> 1) > 0
    if (WIFSIGNALED(status))
    {
        // (status) & 0x7f
        int signal_code = WTERMSIG(status);
        CUTL_ERROR("Child process terminated by signal:" << signal_code <<
                   ", signal msg: " << parse_signal(signal_code));
        return false;
    }

    CUTL_ERROR("unknow error, status: " << status);
    return false;
}

// Reference Doc: https://www.man7.org/linux/man-pages/man3/system.3.html
int call_system_v2(const std::string& cmd)
{
    pid_t status;
    status = std::system(cmd.c_str());

    CUTL_DEBUG("status: " << status);

    // The child process was not created.
    if (-1 == status)
    {
        CUTL_ERROR(
          "Child process could not be created, or its status could not be retrieved. err msg:" <<
          std::strerror(errno));
        return status;
    }

    // (status & 0x7f ) == 0
    // Child process exited normally (exec completed)
    if (WIFEXITED(status))
    {
        // ((status) & 0xff00) >> 8
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0)
        {
            CUTL_WARN("The child process exited normally, exit status:" <<
                      exit_code << ", exit msg: " << parse_exitcode(exit_code));
        }
        return exit_code;
    }

    // Child process terminated by signal
    // ((signed char) (((status) & 0x7f) + 1) >> 1) > 0
    if (WIFSIGNALED(status))
    {
        // (status) & 0x7f
        int signal_code = WTERMSIG(status);
        CUTL_ERROR("Child process terminated by signal:" << signal_code <<
                   ", signal msg: " << parse_signal(signal_code));
        return signal_code;
    }

    // Other unexpected exit status (rare)
    CUTL_ERROR("Unknow error, status: " << status);
    return status;
}

namespace
{

void set_cloexec(int fd)
{
    const int flags = fcntl(fd, F_GETFD);
    if (flags >= 0)
    {
        fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    }
}

void close_fd(int& fd)
{
    if (fd >= 0)
    {
        close(fd);
        fd = -1;
    }
}

} // namespace

command_pipe pipline_open(const std::string& cmd)
{
    command_pipe result;
    int out_fd[2] = {-1, -1};
    int err_fd[2] = {-1, -1};

    if (pipe(out_fd) != 0)
    {
        return result;
    }
    if (pipe(err_fd) != 0)
    {
        const int open_errno = errno;
        close_fd(out_fd[0]);
        close_fd(out_fd[1]);
        errno = open_errno;
        return result;
    }

    set_cloexec(out_fd[0]);
    set_cloexec(out_fd[1]);
    set_cloexec(err_fd[0]);
    set_cloexec(err_fd[1]);

    const pid_t pid = fork();
    if (pid < 0)
    {
        const int open_errno = errno;
        close_fd(out_fd[0]);
        close_fd(out_fd[1]);
        close_fd(err_fd[0]);
        close_fd(err_fd[1]);
        errno = open_errno;
        return result;
    }

    if (pid == 0)
    {
        // 子进程里只调用 async-signal-safe 函数，避免多线程时 fork 后撞上堆锁。
        if (dup2(out_fd[1], STDOUT_FILENO) < 0 || dup2(err_fd[1], STDERR_FILENO) < 0)
        {
            _exit(127);
        }
        close(out_fd[0]);
        close(out_fd[1]);
        close(err_fd[0]);
        close(err_fd[1]);
        execl("/bin/sh", "sh", "-c", cmd.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }

    close_fd(out_fd[1]);
    close_fd(err_fd[1]);

    result.stdout_fp = fdopen(out_fd[0], "r");
    if (result.stdout_fp != nullptr)
    {
        out_fd[0] = -1;
    }
    result.stderr_fp = fdopen(err_fd[0], "r");
    if (result.stderr_fp != nullptr)
    {
        err_fd[0] = -1;
    }
    result.process = static_cast<std::intptr_t>(pid);

    if (result.stdout_fp == nullptr || result.stderr_fp == nullptr)
    {
        const int open_errno = errno;
        if (result.stdout_fp != nullptr)
        {
            fclose(result.stdout_fp);
            result.stdout_fp = nullptr;
        }
        if (result.stderr_fp != nullptr)
        {
            fclose(result.stderr_fp);
            result.stderr_fp = nullptr;
        }
        close_fd(out_fd[0]);
        close_fd(err_fd[0]);
        kill(pid, SIGKILL);
        int status = 0;
        waitpid(pid, &status, 0);
        result.process = 0;
        errno = open_errno;
    }
    return result;
}

int pipline_close(command_pipe& pipe)
{
    if (pipe.stdout_fp != nullptr)
    {
        fclose(pipe.stdout_fp);
        pipe.stdout_fp = nullptr;
    }
    if (pipe.stderr_fp != nullptr)
    {
        fclose(pipe.stderr_fp);
        pipe.stderr_fp = nullptr;
    }
    if (pipe.process == 0)
    {
        return -1;
    }

    const pid_t pid = static_cast<pid_t>(pipe.process);
    pipe.process = 0;
    int status = 0;
    if (waitpid(pid, &status, 0) < 0)
    {
        return -1;
    }
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status))
    {
        return 128 + WTERMSIG(status);
    }
    return status == 0 ? -1 : status;
}

} // namespace cutl

#endif // defined(_WIN32)