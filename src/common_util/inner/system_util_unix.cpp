#if defined(_WIN32)
// do nothing
#else

#include "inner/logger.h"
#include "system_util.h"
#include <csignal>
#include <cstring>
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
          "Child process could not be created, or its status could not be retrieved. err msg:" +
          std::string(std::strerror(errno)));
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
            CUTL_WARN("The child process exited normally, exit status:" +
                      std::to_string(exit_code) + ", exit msg: " + parse_exitcode(exit_code));
            return false;
        }
    }

    // Child process terminated by signal
    // ((signed char) (((status) & 0x7f) + 1) >> 1) > 0
    if (WIFSIGNALED(status))
    {
        // (status) & 0x7f
        int signal_code = WTERMSIG(status);
        CUTL_ERROR("Child process terminated by signal:" + std::to_string(signal_code) +
                   ", signal msg: " + parse_signal(signal_code));
        return false;
    }

    CUTL_ERROR("unknow error, status: " + std::to_string(status));
    return false;
}

// Reference Doc: https://www.man7.org/linux/man-pages/man3/system.3.html
int call_system_v2(const std::string& cmd)
{
    pid_t status;
    status = std::system(cmd.c_str());

    CUTL_DEBUG("status: " + std::to_string(status));

    // The child process was not created.
    if (-1 == status)
    {
        CUTL_ERROR(
          "Child process could not be created, or its status could not be retrieved. err msg:" +
          std::string(std::strerror(errno)));
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
            CUTL_INFO("The child process exited normally, exit status:" +
                      std::to_string(exit_code) + ", exit msg: " + parse_exitcode(exit_code));
        }
        return exit_code;
    }

    // Child process terminated by signal
    // ((signed char) (((status) & 0x7f) + 1) >> 1) > 0
    if (WIFSIGNALED(status))
    {
        // (status) & 0x7f
        int signal_code = WTERMSIG(status);
        CUTL_ERROR("Child process terminated by signal:" + std::to_string(signal_code) +
                   ", signal msg: " + parse_signal(signal_code));
        return signal_code;
    }

    // Other unexpected exit status (rare)
    CUTL_ERROR("Unknow error, status: " + std::to_string(status));
    return status;
}

FILE* pipline_open(const std::string& cmd)
{
    return popen(cmd.c_str(), "r");
}

int pipline_close(FILE* stream)
{
    return pclose(stream);
}

} // namespace cutl

#endif // defined(_WIN32)