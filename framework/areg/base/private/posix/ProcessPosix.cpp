/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/ProcessPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              POSIX specific implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/base/File.hpp"

#include <unistd.h>
#include <stdlib.h>     /* getenv */
#include <iostream>

#ifdef __APPLE__
    #include <mach-o/dyld.h>
#endif // __APPLE__

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

void Process::_os_initilize()
{
    mProcessId = ::getpid( );
    mProcessHandle = static_cast<void *>(&mProcessId);

    char buffer[File::MAXIMUM_PATH];
    ::memset( buffer, 0, File::MAXIMUM_PATH );

#ifdef __APPLE__
    // macOS: use _NSGetExecutablePath to get the executable path
    uint32_t bufSize = static_cast<uint32_t>(File::MAXIMUM_PATH);
    if (_NSGetExecutablePath(buffer, &bufSize) != 0)
    {
        // Buffer too small, try with the required size
        if (bufSize <= File::MAXIMUM_PATH)
        {
            _NSGetExecutablePath(buffer, &bufSize);
        }
    }

    // Resolve any symlinks to get the real path
    char realPath[File::MAXIMUM_PATH];
    if (::realpath(buffer, realPath) != nullptr)
    {
        ::strncpy(buffer, realPath, File::MAXIMUM_PATH - 1);
        buffer[File::MAXIMUM_PATH - 1] = '\0';
    }

#else   // !__APPLE__ (Linux and other POSIX systems)
    constexpr const char _fmtCmdLine[] = "/proc/%lu/cmdline";
    constexpr const char _fmtExePath[] = "/proc/%lu/exe";

    char path[256];
    ::memset( path, 0, 256 );

    sprintf( path, _fmtCmdLine, static_cast<unsigned long>(mProcessId) );
    FILE * file = ::fopen( path, "r" );
    if ( (file == nullptr) || (::fgets( buffer, File::MAXIMUM_PATH, file ) == nullptr))
    {
        sprintf( path, _fmtExePath, static_cast<unsigned long>(mProcessId) );
        ssize_t len = readlink( path, buffer, File::MAXIMUM_PATH );
        if ((len > 0) && (len < File::MAXIMUM_PATH))
        {
            buffer[len] = '\0';
        }
    }

    if (file != nullptr)
    {
        ::fclose(file);
    }
#endif  // __APPLE__

    _init_paths( buffer );
}


String Process::_os_env_variable( const char* var ) const
{
    return String(var != nullptr ? ::getenv(var) : String::EmptyString);
}

} // namespace areg
#endif // defined(_POSIX) || defined(POSIX)
