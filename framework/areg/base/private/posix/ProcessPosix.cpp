/************************************************************************
 * \file        areg/base/private/posix/Process.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              POSIX specific implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#ifdef  _POSIX

#include "system/GEPlatform.h"
#include "areg/base/File.hpp"

#include <unistd.h>
#include <stdlib.h>     /* getenv */

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

Process & Process::initilize( void )
{
    if ( mIsInitialized == false )
    {
        static const char * fmt     = "/proc/%d/cmdline";

        mIsInitialized  = true;
        mProcessId      = getpid();
        mProcessHandle  = static_cast<void *>(&mProcessId);

        char * buffer = DEBUG_NEW char [ File::MAXIMUM_PATH + 1 ];
        if ( buffer != NULL )
        {
            sprintf(buffer, fmt, static_cast<pid_t>(mProcessId));
            FILE * file = fopen(buffer, "r");
            if (file != NULL)
            {
                if (fgets(buffer, File::MAXIMUM_PATH + 1, file) != NULL)
                {
                    _initPaths(buffer);
                }

                fclose(file);
            }

            delete [] buffer;
        }
    }

    return (*this);
}


String Process::getSafeEnvVariable( const char* var ) const
{
    return String(var != NULL ? getenv(var) : String::EmptyString);
}

#endif // _POSIX
