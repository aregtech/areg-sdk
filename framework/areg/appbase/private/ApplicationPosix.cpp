/************************************************************************
* \file        areg/appbase/private/ApplicationWin.cpp
* \ingroup     AREG Asynchronous Event-Driven Communication Framework
* \author      Artak Avetyan
* \brief       AREG Platform, Windows OS specific Application methods implementation
*              Windows apecifix API calls.
************************************************************************/
#include "areg/appbase/Application.hpp"

#if	defined(_POSIX) || defined(POSIX)

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include "areg/base/File.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/trace/GETrace.h"

#include <signal.h>

namespace
{
int _getProcIdByName(const char * procName)
{
    constexpr char const fmt[]        { "/proc/%s/cmdline" };
    constexpr char const dirProc[]    { "/proc" };
    int pid = -1;

    char* buffer = dir != nullptr ? DEBUG_NEW char[File::MAXIMUM_PATH + 1] : nullptr;
    if ((buffer == nullptr) || NEString::isEmpty<char>(procName))
        return pid;

    DIR* dir = opendir(dirProc);
    for (struct dirent* dirEntry = readdir(dir); (pid < 0) && (dirEntry != nullptr); dirEntry = readdir(dir))
    {
        // skip non-numeric directories.
        if ((dirEntry->d_type == DT_REG) && (NEString::isNumeric<char>(dirEntry->d_name[0])))
        {
            String name;
            name.formatString(fmt, dirEntry->d_name);
            FILE* file = fopen(name.getBuffer(), "r");
            if (file != nullptr)
            {
                if (fgets(buffer, File::MAXIMUM_PATH + 1, file) != nullptr)
                {
                    NEString::CharPos pos = NEString::findLast<char>(File::PATH_SEPARATOR, buffer);
                    if (NEString::isPositionValid(pos))
                    {
                        char* name = buffer + pos + 1;
                        if (NEString::compareFastIgnoreCase<char, char>(procName, name) == 0)
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

DEF_TRACE_SCOPE(areg_appbase_ApplicationPosix_handleSignalBrokenPipe);
void handleSignalBrokenPipe(int s)
{
    TRACE_SCOPE(areg_appbase_ApplicationPosix_handleSignalBrokenPipe);
    TRACE_WARN("Caught SIGPIPE signal [ %d ]", s);
}

DEF_TRACE_SCOPE(areg_appbase_ApplicationPosix_handleSignalSegmentationFault);
void handleSignalSegmentationFault(int s)
{
    TRACE_SCOPE(areg_appbase_ApplicationPosix_handleSignalSegmentationFault);
    TRACE_ERR("Caught segmantion fault!!! Parameter [ %d ]", s);
}

} // namespace

void Application::setupHandlers( void )
{
    Application & theApp = Application::getInstance();
    if ( theApp.mSetup == false )
    {

        signal(SIGPIPE, &handleSignalBrokenPipe);
        signal(SIGSEGV, &handleSignalSegmentationFault);
    }

    theApp.mSetup = true;
}

/**
* \brief   POSIX specific implementation of method.
**/
bool Application::_startRouterService( void )
{
    int pid = _getProcIdByName(NEApplication::DEFAULT_ROUTER_SERVICE_NAME.data());
    bool result = pid != -1;
    if (pid < 0)
    {
        constexpr char const argv0[] { "--console" };
        
        String fileName = String::EmptyString;
        fileName.append(Process::getInstance().getPath())
                .append(File::PATH_SEPARATOR)
                .append(NEApplication::DEFAULT_ROUTER_SERVICE_NAME);

        result = execl(fileName.getString(), fileName.getString(), argv0, nullptr) > 0;
    }

    return result;
}

#endif // defined(_POSIX) || defined(POSIX)
