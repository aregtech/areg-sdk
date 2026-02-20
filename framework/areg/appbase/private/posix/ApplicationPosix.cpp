/************************************************************************
* \file        areg/appbase/private/ApplicationWin.cpp
* \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
* \author      Artak Avetyan
* \brief       Areg Platform, Windows OS specific Application methods implementation
*              Windows apecifix API calls.
************************************************************************/
#include "areg/appbase/Application.hpp"

#if	defined(_POSIX) || defined(POSIX)

#include "areg/base/File.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/GELog.h"

#include <dirent.h>
#include <stdio.h>
#include <signal.h>

#ifdef __APPLE__
    #include <libproc.h>
#endif  // __APPLE__

namespace
{


#ifdef __APPLE__    //macOS

    int _getProcIdByName(const char* procName)
    {
        if (NEString::isEmpty<char>(procName))
            return -1;

        // macOS implementation using sysctl
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
        size_t size  {0};

        if (sysctl(mib, 4, nullptr, &size, nullptr, 0) < 0)
            return -1;

        uint8_t* buffer = size != 0 ? DEBUG_NEW uint8_t[size] : nullptr;
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
            if (NEString::compareIgnoreCase<char, char>(procName, procs[i].kp_proc.p_comm) == NEMath::Ordering::Equal)
            {
                pid = procs[i].kp_proc.p_pid;
            }
        }

        delete[] buffer;
        return pid;
    }

#else   // Linux

    int _getProcIdByName(const char* procName)
    {
        if (NEString::isEmpty<char>(procName))
            return -1;

        // Linux implementation using /proc
        constexpr char const fmt[]{ "/proc/%s/cmdline" };
        constexpr char const dirProc[]{ "/proc" };
        int pid {-1};

        DIR* dir = opendir(dirProc);
        char* buffer = dir != nullptr ? DEBUG_NEW char[File::MAXIMUM_PATH + 1] : nullptr;
        if (buffer == nullptr)
            return pid;

        for (struct dirent* dirEntry = readdir(dir); (pid < 0) && (dirEntry != nullptr); dirEntry = readdir(dir))
        {
            if (NEString::isNumeric<char>(dirEntry->d_name[0]))
            {
                String name;
                name.format(fmt, dirEntry->d_name);
                FILE* file = fopen(name.getBuffer(), "r");
                if (file != nullptr)
                {
                    if (fgets(buffer, File::MAXIMUM_PATH + 1, file) != nullptr)
                    {
                        NEString::CharPos pos = NEString::findLast<char>(File::PATH_SEPARATOR, buffer);
                        if (NEString::isPositionValid(pos))
                        {
                            char* procPath = buffer + pos + 1;
                            if (NEString::compareIgnoreCase<char, char>(procName, procPath) == NEMath::Ordering::Equal)
                            {
                                pid = NEString::makeInteger<char>(dirEntry->d_name, nullptr);
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

    DEF_LOG_SCOPE(areg_appbase_ApplicationPosix__handleSignalBrokenPipe);
    void _handleSignalBrokenPipe(int s)
    {
        LOG_SCOPE(areg_appbase_ApplicationPosix__handleSignalBrokenPipe);
        LOG_WARN("Caught SIGPIPE signal [ %d ]", s);
    }

    DEF_LOG_SCOPE(areg_appbase_ApplicationPosix__handleSignalSegmentationFault);
    void _handleSignalSegmentationFault(int s)
    {
        LOG_SCOPE(areg_appbase_ApplicationPosix__handleSignalSegmentationFault);
        LOG_ERR("Caught segmentation fault!!! Parameter [ %d ]", s);
    }

} // namespace

void Application::_osSetupHandlers()
{
    Application & theApp = Application::getInstance();
    Lock lock(theApp.mLock);

    if ( theApp.mSetup == false )
    {

        signal(SIGPIPE, &_handleSignalBrokenPipe);
        signal(SIGSEGV, &_handleSignalSegmentationFault);

        theApp.mSetup = true;
    }
}

void Application::_osReleaseHandlers()
{
    Application& theApp = Application::getInstance();
    Lock lock(theApp.mLock);

    if (theApp.mSetup)
    {
        signal(SIGPIPE, nullptr);
        signal(SIGSEGV, nullptr);
        theApp.mSetup = false;
    }
}

/**
 * \brief   Windows OS specific implementation of method.
 **/
bool Application::_osStartLocalService(const wchar_t* serviceName, const wchar_t* serviceExecutable)
{
    ASSERT(NEString::isEmpty<wchar_t>(serviceName) == false);
    ASSERT(NEString::isEmpty<wchar_t>(serviceExecutable) == false);
    String serviceExe(serviceExecutable);
    int pid = _getProcIdByName(serviceExe);
    bool result{ pid > 0 };
    if (pid < 0)
    {
        constexpr std::string_view fmt { "systemctl start %s" };
        char cmd[512];
        String::formatString(cmd, 512, fmt.data(), serviceName);
        result = std::system(cmd);
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
