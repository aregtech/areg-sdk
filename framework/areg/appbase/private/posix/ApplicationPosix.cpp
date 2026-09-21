/************************************************************************
* \file        areg/appbase/private/ApplicationWin.cpp
* \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
* \author      Artak Avetyan
* \brief       Areg Platform, Windows OS specific Application methods implementation
*              Windows apecifix API calls.
************************************************************************/

#if	defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/areg_log.h"

#include <dirent.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#ifdef __APPLE__
    #include <libproc.h>
#endif  // __APPLE__

namespace {

#ifdef __APPLE__    //macOS

    int32_t _getProcIdByName(const char* procName)
    {
        if (areg::is_empty<char>(procName))
            return -1;

        // macOS implementation using sysctl
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
        size_t size  {0};

        if (sysctl(mib, 4, nullptr, &size, nullptr, 0) < 0)
            return -1;

        uint8_t* buffer = size != 0 ? new uint8_t[size] : nullptr;
        if (buffer == nullptr)
            return -1;

        struct kinfo_proc* procs = reinterpret_cast<struct kinfo_proc*>(buffer);
        if (sysctl(mib, 4, procs, &size, nullptr, 0) < 0)
        {
            delete[] buffer;
            return -1;
        }

        int count = static_cast<int>(size / sizeof(struct kinfo_proc));
        int pid {-1};

        for (int i = 0; i < count && pid < 0; ++i)
        {
            if (areg::compare_ignore_case<char, char>(procName, procs[i].kp_proc.p_comm) == areg::Ordering::Equal)
            {
                pid = procs[i].kp_proc.p_pid;
            }
        }

        delete[] buffer;
        return pid;
    }

#else   // Linux

    int32_t _getProcIdByName(const char* procName)
    {
        if (areg::is_empty<char>(procName))
            return -1;

        // Linux implementation using /proc
        constexpr char const fmt[]{ "/proc/%s/cmdline" };
        constexpr char const dirProc[]{ "/proc" };
        int pid {-1};

        DIR* dir = opendir(dirProc);
        char* buffer = dir != nullptr ? new char[areg::File::MAXIMUM_PATH + 1] : nullptr;
        if (buffer == nullptr)
            return pid;

        for (struct dirent* dirEntry = readdir(dir); (pid < 0) && (dirEntry != nullptr); dirEntry = readdir(dir))
        {
            if (areg::is_numeric<char>(dirEntry->d_name[0]))
            {
                areg::String name;
                name.format(fmt, dirEntry->d_name);
                FILE* file = fopen(name.as_string(), "r");
                if (file != nullptr)
                {
                    if (fgets(buffer, areg::File::MAXIMUM_PATH + 1, file) != nullptr)
                    {
                        areg::CharPos pos = areg::find_last<char>(areg::File::PATH_SEPARATOR, buffer);
                        if (areg::is_position_valid(pos))
                        {
                            char* procPath = buffer + pos + 1;
                            if (areg::compare_ignore_case<char, char>(procName, procPath) == areg::Ordering::Equal)
                            {
                                pid = areg::make_integer<char>(dirEntry->d_name, nullptr);
                            }
                        }
                    }

                    fclose(file);
                }
            }
        }

        delete[] buffer;
        closedir(dir);
        return pid;
    }
#endif  // Linux

    //!< The dispositions replaced by _os_setup_handlers(), put back by _os_release_handlers().
    struct sigaction    _prevPipeAction { };
    struct sigaction    _prevSegvAction { };
    bool                _ownsPipeSignal { false };
    bool                _ownsSegvSignal { false };

    void _handle_signal_broken_pipe(int32_t)
    {
    }

    void _handle_signal_segmentation_fault(int32_t s)
    {
        // Only calls that a signal handler may make: no logging, no allocation, no lock.
        constexpr char message[]{ "areg: caught a segmentation fault\n" };
        [[maybe_unused]] ssize_t written = ::write(STDERR_FILENO, message, sizeof(message) - 1u);

        // Restore the default handler and re-raise so the kernel terminates the
        // process, closes all sockets, and generates a core dump.
        ::signal(s, SIG_DFL);
        ::raise(s);
    }

    /**
     * \brief   Installs \a handler for \a signum only while the signal has no owner. A sanitizer,
     *          a debugger or the host application that already handles it keeps it.
     *
     * \param   signum      The signal to take over.
     * \param   handler     The handler to install.
     * \param   out_prev    On return, the disposition that was replaced.
     * \return  Returns true if the handler was installed.
     **/
    bool _take_signal(int signum, void (*handler)(int32_t), struct sigaction & out_prev)
    {
        struct sigaction current;
        areg::mem_zero(static_cast<void *>(&current), sizeof(current));
        if (::sigaction(signum, nullptr, &current) != 0)
            return false;

        const bool taken{ ((current.sa_flags & SA_SIGINFO) != 0) ||
                          ((current.sa_handler != SIG_DFL) && (current.sa_handler != SIG_IGN)) };
        if (taken)
            return false;

        struct sigaction action;
        areg::mem_zero(static_cast<void *>(&action), sizeof(action));
        action.sa_handler = handler;
        action.sa_flags = SA_RESTART;
        // Unqualified: sigemptyset() is a macro on macOS.
        sigemptyset(&action.sa_mask);

        return (::sigaction(signum, &action, &out_prev) == 0);
    }

} // namespace

void areg::Application::_os_setup_handlers()
{
    Application & theApp = Application::instance();
    Lock lock(theApp.mLock);

    if ( theApp.mSetup == false )
    {
        _ownsPipeSignal = _take_signal(SIGPIPE, &_handle_signal_broken_pipe, _prevPipeAction);
        _ownsSegvSignal = _take_signal(SIGSEGV, &_handle_signal_segmentation_fault, _prevSegvAction);

        theApp.mSetup = true;
    }
}

void areg::Application::_os_release_handlers()
{
    Application& theApp = Application::instance();
    Lock lock(theApp.mLock);

    if (theApp.mSetup)
    {
        if (_ownsPipeSignal)
        {
            ::sigaction(SIGPIPE, &_prevPipeAction, nullptr);
            _ownsPipeSignal = false;
        }

        if (_ownsSegvSignal)
        {
            ::sigaction(SIGSEGV, &_prevSegvAction, nullptr);
            _ownsSegvSignal = false;
        }

        theApp.mSetup = false;
    }
}

/**
 * \brief   Windows OS specific implementation of method.
 **/
bool areg::Application::_os_start_local_service(const wchar_t* serviceName, const wchar_t* serviceExecutable)
{
    ASSERT(areg::is_empty<wchar_t>(serviceName) == false);
    ASSERT(areg::is_empty<wchar_t>(serviceExecutable) == false);
    String serviceExe(serviceExecutable);
    int32_t pid = _getProcIdByName(serviceExe);
    bool result{ pid > 0 };
    if (pid < 0)
    {
        constexpr std::string_view fmt { "systemctl start %s" };
        char cmd[512];
        String::format_string(cmd, 512, fmt.data(), serviceName);
        result = std::system(cmd);
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
