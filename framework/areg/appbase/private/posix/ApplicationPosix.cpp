/************************************************************************
* \file        areg/appbase/private/posix/ApplicationPosix.cpp
* \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
* \author      Artak Avetyan
* \brief       Areg Platform, POSIX specific Application methods implementation.
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

#include <errno.h>
#include <fcntl.h>
#include <new>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <string_view>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __APPLE__
    #include <crt_externs.h>
    #include <libproc.h>
    #include <sys/param.h>
#else   // __APPLE__
    #include <dirent.h>
    extern char** environ;
#endif  // __APPLE__

namespace {

    //!< Returns true if the executable name is the base name, with or without an extension.
    bool _is_service_executable(std::string_view name, std::string_view baseName) noexcept
    {
        return (name.size() >= baseName.size())
            && (name.compare(0u, baseName.size(), baseName) == 0)
            && ((name.size() == baseName.size()) || (name[baseName.size()] == '.'));
    }

#ifdef __APPLE__

    //!< Returns true if a process with the given executable base name runs.
    bool _is_process_running(std::string_view baseName) noexcept
    {
        const int count{ ::proc_listallpids(nullptr, 0) };
        if (count <= 0)
            return false;

        const int capacity{ count + 32 };
        pid_t* const pids{ new (std::nothrow) pid_t[static_cast<size_t>(capacity)] };
        if (pids == nullptr)
            return false;

        const int found{ ::proc_listallpids(pids, capacity * static_cast<int>(sizeof(pid_t))) };
        char name[2 * MAXCOMLEN + 1]{ };
        bool result{ false };
        for (int i = 0; (i < found) && (result == false); ++i)
        {
            if ((pids[i] > 0) && (::proc_name(pids[i], name, sizeof(name)) > 0))
            {
                result = _is_service_executable(name, baseName);
            }
        }

        delete[] pids;
        return result;
    }

    //!< The environment of the calling process.
    char** _process_environment() noexcept
    {
        return *::_NSGetEnviron();
    }

#else   // __APPLE__

    //!< Returns true if a process with the given executable base name runs.
    bool _is_process_running(std::string_view baseName) noexcept
    {
        DIR* const dir{ ::opendir("/proc") };
        if (dir == nullptr)
            return false;

        char path[64]{ };
        char cmdline[1024]{ };
        bool result{ false };
        for (struct dirent* entry = ::readdir(dir); (entry != nullptr) && (result == false); entry = ::readdir(dir))
        {
            if ((entry->d_name[0] < '1') || (entry->d_name[0] > '9'))
                continue;

            const int length{ ::snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name) };
            if ((length <= 0) || (length >= static_cast<int>(sizeof(path))))
                continue;

            const int fd{ ::open(path, O_RDONLY | O_CLOEXEC) };
            if (fd < 0)
                continue;

            const ssize_t size{ ::read(fd, cmdline, sizeof(cmdline) - 1u) };
            ::close(fd);
            if (size <= 0)
                continue;

            cmdline[size] = '\0';
            const std::string_view argv0{ cmdline };
            const size_t slash{ argv0.rfind('/') };
            result = _is_service_executable(slash == std::string_view::npos ? argv0 : argv0.substr(slash + 1u), baseName);
        }

        ::closedir(dir);
        return result;
    }

    //!< The environment of the calling process.
    char** _process_environment() noexcept
    {
        return ::environ;
    }

#endif  // __APPLE__

    //!< Runs the command without a shell and returns true if it exits with status 0.
    bool _run_command(char* const argv[]) noexcept
    {
        pid_t child{ -1 };
        if (::posix_spawnp(&child, argv[0], nullptr, nullptr, argv, _process_environment()) != 0)
            return false;

        int status{ 0 };
        pid_t waited{ -1 };
        do
        {
            waited = ::waitpid(child, &status, 0);
        } while ((waited < 0) && (errno == EINTR));

        return (waited == child) && WIFEXITED(status) && (WEXITSTATUS(status) == 0);
    }

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
 * \brief   POSIX specific implementation: systemd on Linux, launchd on macOS.
 **/
bool areg::Application::_os_start_local_service(areg::RemoteServiceKind service)
{
    std::string_view baseName{ };
    if (service == areg::RemoteServiceKind::Router)
        baseName = areg::DEFAULT_ROUTER_SERVICE_NAME;
    else if (service == areg::RemoteServiceKind::Logger)
        baseName = areg::DEFAULT_LOGGER_SERVICE_NAME;
    else
        return false;

    if (_is_process_running(baseName))
        return true;

    char target[64]{ };
#ifdef __APPLE__
    const int length{ ::snprintf(target, sizeof(target), "system/tech.areg.%.*s", static_cast<int>(baseName.size()), baseName.data()) };
    char program[]{ "launchctl" };
    char command[]{ "kickstart" };
    char* const argv[]{ program, command, target, nullptr };
#else   // __APPLE__
    const int length{ ::snprintf(target, sizeof(target), "%.*s.service", static_cast<int>(baseName.size()), baseName.data()) };
    char program[]{ "systemctl" };
    char noPrompt[]{ "--no-ask-password" };
    char command[]{ "start" };
    char* const argv[]{ program, noPrompt, command, target, nullptr };
#endif  // __APPLE__

    if ((length <= 0) || (length >= static_cast<int>(sizeof(target))))
        return false;

    return _run_command(argv);
}

#endif // defined(_POSIX) || defined(POSIX)
