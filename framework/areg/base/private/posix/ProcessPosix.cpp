/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/Process.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              POSIX specific implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#if defined(_POSIX) || defined(POSIX)

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
        mIsInitialized = true;
        mProcessId = ::getpid( );
        mProcessHandle = static_cast<void *>(&mProcessId);

        char buffer[File::MAXIMUM_PATH];
        char path[256];

        buffer[0] = path[0] = '\0';

        sprintf( path, "/proc/%lu/cmdLine", static_cast<uint64_t>(mProcessId) );
        FILE * file = ::fopen( path, "r" );
        if ( (file == nullptr) || (::fgets( buffer, File::MAXIMUM_PATH, file ) == nullptr))
        {
            sprintf( path, "/proc/%lu/exe", static_cast<uint64_t>(mProcessId) );
            readlink( path, buffer, File::MAXIMUM_PATH );
        }

        if (file != nullptr)
        {
            ::fclose(file);
        }

        _initPaths( buffer );
    }

    return (*this);
}


String Process::getSafeEnvVariable( const char* var ) const
{
    return String(var != nullptr ? ::getenv(var) : String::EmptyString);
}

#endif // defined(_POSIX) || defined(POSIX)
