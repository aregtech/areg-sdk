/************************************************************************
* \file        areg/appbase/private/ApplicationWin.cpp
* \ingroup     AREG Asynchronous Event-Driven Communication Framework
* \author      Artak Avetyan
* \brief       AREG Platform, Windows OS specific Application methods implementation
*              Windows apecifix API calls.
************************************************************************/
#include "areg/appbase/Application.hpp"

#if	defined(_POSIX) || defined(POSIX)

#include "areg/base/File.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/trace/GETrace.h"

#include <dirent.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

namespace
{
    int _getProcIdByName(const char * procName)
    {
        constexpr char const fmt[]        { "/proc/%s/cmdline" };
        constexpr char const dirProc[]    { "/proc" };
        int pid = -1;

        DIR* dir = opendir(dirProc);
        char* buffer = dir != nullptr ? DEBUG_NEW char[File::MAXIMUM_PATH + 1] : nullptr;
        if ((buffer == nullptr) || NEString::isEmpty<char>(procName))
            return pid;

        for (struct dirent* dirEntry = readdir(dir); (pid < 0) && (dirEntry != nullptr); dirEntry = readdir(dir))
        {
            // skip non-numeric directories.
            if ((dirEntry->d_type == DT_REG) && (NEString::isNumeric<char>(dirEntry->d_name[0])))
            {
                String name;
                name.format(fmt, dirEntry->d_name);
                FILE* file = fopen(name.getBuffer(), "r");
                if (file != nullptr)
                {
                    if (fgets(buffer, File::MAXIMUM_PATH + 1, file) != nullptr)
                    {
                        NEString::CharPos pos = NEString::findLast<char>(File::getPathSeparator(), buffer);
                        if (NEString::isPositionValid(pos))
                        {
                            char* name = buffer + pos + 1;
                            if (NEString::compareFastIgnoreCase<char, char>(procName, name) == NEMath::eCompare::Equal)
                            {
                                pid = NEString::makeInteger<char>(dirEntry->d_name, nullptr);
                            }
                        }
                    }

                    fclose(file);
                }
            }
        }

        return pid;
    }

    DEF_TRACE_SCOPE(areg_appbase_ApplicationPosix__handleSignalBrokenPipe);
    void _handleSignalBrokenPipe(int s)
    {
        TRACE_SCOPE(areg_appbase_ApplicationPosix__handleSignalBrokenPipe);
        TRACE_WARN("Caught SIGPIPE signal [ %d ]", s);
    }

    DEF_TRACE_SCOPE(areg_appbase_ApplicationPosix__handleSignalSegmentationFault);
    void _handleSignalSegmentationFault(int s)
    {
        TRACE_SCOPE(areg_appbase_ApplicationPosix__handleSignalSegmentationFault);
        TRACE_ERR("Caught segmentation fault!!! Parameter [ %d ]", s);
    }

} // namespace

void Application::_osSetupHandlers( void )
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

void Application::_osReleaseHandlers(void)
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
* \brief   POSIX specific implementation of method.
**/
bool Application::_osStartRouterService( void )
{
    int pid = _getProcIdByName(NEApplication::DEFAULT_ROUTER_SERVICE_NAME.data());
    bool result = pid != -1;
    if (pid < 0)
    {
        constexpr char const argv0[] { "--console" };
        
        String fileName = String::EmptyString;
        fileName.append(Process::getInstance().getPath())
                .append(File::getPathSeparator())
                .append(NEApplication::DEFAULT_ROUTER_SERVICE_NAME);

        result = execl(fileName.getString(), fileName.getString(), argv0, nullptr) > 0;
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
