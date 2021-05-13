/************************************************************************
 * \file        areg/base/private/win32/Process32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              Windows specific implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#ifdef	_WINDOWS

#include "areg/base/WideString.hpp"
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

Process::Process( void )
    : mProcessHandle    ( static_cast<void *>(::GetCurrentProcess()) )
    , mProcessId        ( ::GetCurrentProcessId() )
    , mAppName          ( )
    , mProcessName      ( )
    , mProcessExt       ( )
    , mProcessPath      ( )
    , mProcessFullPath  ( )
{
    TCHAR fullPath[ MAX_PATH + 1 ];
    NEMemory::zeroBuffer(fullPath, (MAX_PATH + 1) * sizeof(TCHAR));

    if ( ::GetModuleFileNameEx( (HANDLE)(mProcessHandle), NULL, fullPath, MAX_PATH) != 0 )
    {
        String temp = fullPath;
        _initPaths(temp.getString());
    }
}

String Process::getSafeEnvVariable( const char* var ) const
{
    String result;
    DWORD length = ::GetEnvironmentVariableA(var, NULL, 0);
    if (length > 0)
    {
        char *buffer = new char[length+1];
        ::GetEnvironmentVariableA(var, buffer, length);
        result = buffer;
        delete[] buffer;
    }
    return result;
}

#endif // _WINDOWS
