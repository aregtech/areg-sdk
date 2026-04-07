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
 * \brief       Areg Platform, POSIX process implementation.
 *              _os_initilize: uses getpid() + readlink("/proc/self/exe") for
 *              all non-Apple POSIX platforms (Linux, Cygwin, etc.).
 *              _os_env_variable: uses getenv(), common to all POSIX platforms.
 *              macOS uses _NSGetExecutablePath — see macos/ProcessMacOS.cpp.
 *              ProcessLinux.cpp is reserved for future Linux-specific optimizations.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include <stdlib.h>     // getenv
#include <unistd.h>     // readlink, getpid
#include <cstring>

//////////////////////////////////////////////////////////////////////////
// Non-Apple POSIX: _os_initilize using getpid() + readlink("/proc/self/exe")
//////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__

namespace areg {

void Process::_os_initilize()
{
    mProcessId     = ::getpid();
    mProcessHandle = static_cast<void *>(&mProcessId);

    char buffer[File::MAXIMUM_PATH];
    ::memset(buffer, 0, File::MAXIMUM_PATH);

    // /proc/self/exe is available on Linux and Cygwin.
    // readlink returns -1 silently on systems that lack it; buffer stays empty.
    ssize_t len = ::readlink("/proc/self/exe", buffer, File::MAXIMUM_PATH - 1);
    if ((len > 0) && (len < File::MAXIMUM_PATH))
    {
        buffer[len] = '\0';
    }

    _init_paths(buffer);
}

} // namespace areg

#endif  // !__APPLE__

//////////////////////////////////////////////////////////////////////////
// Common POSIX: _os_env_variable — shared by all POSIX platforms
//////////////////////////////////////////////////////////////////////////

namespace areg {

String Process::_os_env_variable(const char * var) const
{
    return String(var != nullptr ? ::getenv(var) : String::EmptyString);
}

} // namespace areg

#endif  // defined(_POSIX) || defined(POSIX)
