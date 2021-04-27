/************************************************************************
 * \file        areg/base/private/posix/CEProcess.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              POSIX specific implementation
 ************************************************************************/
#include "areg/base/CEProcess.hpp"

#ifdef  _POSIX

#include "system/GEPlatform.h"
#include "areg/base/CEFile.hpp"

#include <unistd.h>
#include <stdlib.h>     /* getenv */

//////////////////////////////////////////////////////////////////////////
// CEProcess class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEProcess::CEProcess( void )
    : mProcessId        ( getpid() )
    , mProcessHandle    ( static_cast<void *>(&mProcessId) )
    , mAppName          ( )
    , mProcessName      ( )
    , mProcessExt       ( )
    , mProcessPath      ( )
    , mProcessFullPath  ( )
{
    static const char * fmt     = "/proc/%d/cmdline";

    char * buffer = DEBUG_NEW char [ CEFile::MAXIMUM_PATH + 1 ];
    if ( buffer != NULL )
    {
        sprintf(buffer, fmt, static_cast<pid_t>(mProcessId));
        FILE * file = fopen(buffer, "r");
        if (file != NULL)
        {
            if (fgets(buffer, CEFile::MAXIMUM_PATH + 1, file) != NULL)
            {
                initPaths(buffer);
            }

            fclose(file);
        }

        delete [] buffer;
    }
}

CEString CEProcess::GetSafeEnvVariable( const char* var ) const
{
    return CEString(var != NULL ? getenv(var) : CEString::EmptyString);
}

#endif // _POSIX
