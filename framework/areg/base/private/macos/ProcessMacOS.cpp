/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/macos/ProcessMacOS.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, macOS process initialization.
 *              Reads executable path via Mach-O dyld API.
 *
 ************************************************************************/
#ifdef __APPLE__

#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"
#include <unistd.h>
#include <mach-o/dyld.h>
#include <cstring>
#include <cstdlib>

namespace areg {

void Process::_os_initilize()
{
    mProcessId     = ::getpid();
    mProcessHandle = static_cast<void *>(&mProcessId);

    char buffer[File::MAXIMUM_PATH];
    ::memset(buffer, 0, File::MAXIMUM_PATH);

    uint32_t bufSize = static_cast<uint32_t>(File::MAXIMUM_PATH);
    if (_NSGetExecutablePath(buffer, &bufSize) != 0)
    {
        // Buffer too small — try again with the required size.
        if (bufSize <= static_cast<uint32_t>(File::MAXIMUM_PATH))
        {
            _NSGetExecutablePath(buffer, &bufSize);
        }
    }

    // Resolve symlinks to get the canonical path.
    char realPath[File::MAXIMUM_PATH];
    if (::realpath(buffer, realPath) != nullptr)
    {
        ::strncpy(buffer, realPath, File::MAXIMUM_PATH - 1);
        buffer[File::MAXIMUM_PATH - 1] = '\0';
    }

    _init_paths(buffer);
}

} // namespace areg

#endif  // __APPLE__
