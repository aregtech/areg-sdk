/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/Process32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              Windows specific implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#ifdef	_WINDOWS

#include "areg/base/File.hpp"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <psapi.h>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

void Process::_osInitilize( void )
{
    mProcessId      = ::GetCurrentProcessId();
    mProcessHandle	= static_cast<void *>(::GetCurrentProcess());

    TCHAR fullPath[File::MAXIMUM_PATH];
    NEMemory::memZero(fullPath, (File::MAXIMUM_PATH) * sizeof(TCHAR));

    if ( ::GetModuleFileNameEx( (HANDLE)(mProcessHandle), nullptr, fullPath, MAX_PATH) != 0 )
    {
        String temp(fullPath);
        _initPaths(temp.getString());
    }
}


String Process::_osGetEnvVariable( const char* var ) const
{
    String result;
    uint32_t length = var != nullptr ? static_cast<uint32_t>(::GetEnvironmentVariableA(var, nullptr, 0)) : 0;
    uint32_t size = length + 1u;
    if (size > 1)
    {
        char *buffer = DEBUG_NEW char[size];
        ::GetEnvironmentVariableA(var, buffer, length);
        result = buffer;
        delete [] buffer;
    }

    return result;
}

#endif // _WINDOWS
