#if defined(_WIN32)

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <io.h>
#include <fcntl.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "system_util.h"
#include "inner/logger.h"

namespace cutl
{

    // https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/system-wsystem?view=msvc-170
    bool call_system(const std::string &cmd)
    {
        if (cmd.empty())
        {
            CUTL_ERROR("cmd is empty!");
            return false;
        }

        int ret = system(cmd.c_str());
        if (ret != 0)
        {
            CUTL_ERROR("system failure, error" << strerror(errno));
            return false;
        }

        return true;
    }

    bool call_system_v2(const std::string& cmd)
    {
        return system(cmd.c_str());
    }

    namespace
    {

    void close_handle(HANDLE& handle)
    {
        if (handle != NULL && handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
            handle = NULL;
        }
    }

    FILE* handle_to_file(HANDLE& handle)
    {
        if (handle == NULL || handle == INVALID_HANDLE_VALUE)
        {
            return nullptr;
        }

        const int fd = _open_osfhandle(reinterpret_cast<intptr_t>(handle), _O_RDONLY | _O_TEXT);
        if (fd < 0)
        {
            close_handle(handle);
            return nullptr;
        }
        // fd 已接管 handle，失败时不能再 CloseHandle。
        handle = NULL;

        FILE* fp = _fdopen(fd, "r");
        if (fp == nullptr)
        {
            _close(fd);
        }
        return fp;
    }

    } // namespace

    // stdout、stderr 分管道。命令仍交给 cmd.exe，和原先的 _popen 一致。
    command_pipe pipline_open(const std::string &cmd)
    {
        command_pipe result;

        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;

        HANDLE out_read = NULL;
        HANDLE out_write = NULL;
        HANDLE err_read = NULL;
        HANDLE err_write = NULL;
        if (!CreatePipe(&out_read, &out_write, &sa, 0))
        {
            errno = EIO;
            return result;
        }
        if (!CreatePipe(&err_read, &err_write, &sa, 0))
        {
            close_handle(out_read);
            close_handle(out_write);
            errno = EIO;
            return result;
        }
        SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        si.hStdOutput = out_write;
        si.hStdError = err_write;

        std::string cmdline = "cmd.exe /c " + cmd;
        PROCESS_INFORMATION pi{};
        const BOOL created = CreateProcessA(
          NULL, &cmdline[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
        const DWORD create_error = created ? 0 : GetLastError();
        close_handle(out_write);
        close_handle(err_write);
        if (!created)
        {
            CUTL_ERROR("CreateProcess failed, GetLastError=" << create_error << ", cmd: " << cmd);
            close_handle(out_read);
            close_handle(err_read);
            errno = EIO;
            return result;
        }
        CloseHandle(pi.hThread);

        result.stdout_fp = handle_to_file(out_read);
        result.stderr_fp = handle_to_file(err_read);
        result.process = reinterpret_cast<std::intptr_t>(pi.hProcess);
        if (result.stdout_fp == nullptr || result.stderr_fp == nullptr)
        {
            const int open_errno = errno != 0 ? errno : EIO;
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
            close_handle(out_read);
            close_handle(err_read);
            TerminateProcess(pi.hProcess, 1);
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            result.process = 0;
            errno = open_errno;
        }
        return result;
    }

    int pipline_close(command_pipe &pipe)
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

        HANDLE process = reinterpret_cast<HANDLE>(pipe.process);
        pipe.process = 0;
        const DWORD wait_result = WaitForSingleObject(process, INFINITE);
        DWORD exit_code = 1;
        const BOOL got_code = (wait_result == WAIT_OBJECT_0) && GetExitCodeProcess(process, &exit_code);
        CloseHandle(process);
        if (!got_code)
        {
            return -1;
        }
        return static_cast<int>(exit_code);
    }

} // namespace cutl

#endif // defined(_WIN32)