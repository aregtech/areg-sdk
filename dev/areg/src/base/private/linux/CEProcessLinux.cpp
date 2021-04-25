/************************************************************************
 * \file        areg/src/base/private/linux/CEProcess.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              Linux specific implementation
 ************************************************************************/
#include "areg/src/base/CEProcess.hpp"

#ifdef	_LINUX

#include "system/src/GEPlatform.h"
#include "areg/src/base/CEFile.hpp"

#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
// CEProcess class implementation
//////////////////////////////////////////////////////////////////////////

CEProcess & CEProcess::GetProcess( void )
{
    static CEProcess    _process;
    return _process;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

CEProcess::CEProcess( void )
    : mProcessHandle    ( NULL /* no such thing in Linux */ )
    , mProcessId        ( getpid() )
    , mGlobalModuleId   ( getpid() )
{
    memset(mProcessName     , 0, CEProcess::MAX_PROCESS_NAME * sizeof(char));
    memset(mProcessExt      , 0, CEProcess::MAX_PROCESS_EXT  * sizeof(char));
    memset(mProcessPath     , 0, CEProcess::MAX_PROCESS_PATH * sizeof(char));
    memset(mProcessFullPath , 0, CEProcess::MAX_PROCESS_PATH * sizeof(char));

    // according to many sources, the best way to get the process name is using argv[0]
    // this _can_ be accessed in a library by using the section ".init_array"
    // (see https://stackoverflow.com/questions/2471553/access-command-line-arguments-without-using-char-argv-in-main/24718544#24718544)
    // however, Linux provides program_invocation_name which can be used if available
    // (see http://linux.die.net/man/3/program_invocation_name)
    // program_invocation_name is _exactly_ argv[0], so it might be something like ./program_name
    // --> create full path with CEFile
    // mProcessName and mProcessExt can be taken from program_invocation_short_name, where the path is already cut off
    NEStd::StringCopyA(mProcessName, CEProcess::MAX_PROCESS_NAME, program_invocation_short_name);
    char* ext = strrchr(mProcessName, '.');
    if ( ext != NULL )
    {
        *ext ++ = '\0';
        NEStd::StringCopyA(mProcessExt, CEProcess::MAX_PROCESS_EXT, ext);
    }
    // to get the full path to the file, we need to use CEFile:
    // WARNING: when program_invocation_name is a path relative to the directory where the process was started,
    // CEFile::GetFileFullPath will FAIL if the current director has been change since start of the program!
    // TODO: is this an issue? should be use .init_array (above) to store the starting directory?
    std::string fullPath = CEFile::GetFileFullPath(program_invocation_name);
    if (!fullPath.empty()) {
        NEStd::StringCopyA(mProcessFullPath, CEProcess::MAX_PROCESS_PATH, fullPath.c_str());
    }
    else {
        // uh oh? use what we got because it'S better than nothing?
        NEStd::StringCopyA(mProcessFullPath, CEProcess::MAX_PROCESS_PATH, program_invocation_name);
    }
    NCZStd::StringCopyA(mProcessPath, CEProcess::MAX_PROCESS_PATH, fullPath.c_str());
    char * name = strrchr(mProcessPath, CZFile::PATH_SEPARATOR);
    if ( name != NULL )
    {
        *name = '\0';
    }
    else
    {
        mProcessPath[0] = '\0';
    }
}

CEProcess::~CEProcess( void )
{
    mProcessHandle      = NULL;
    mProcessId          = 0;
    mGlobalModuleId     = 0;
    mProcessName[0]     = '\0';
    mProcessExt[0]      = '\0';
    mProcessPath[0]     = '\0';
    mProcessFullPath[0] = '\0';
}

std::string CEProcess::GetSafeEnvVariable( const char* var ) const
{
    std::string result;

    // return empty string if process has different effective user or group
    if (getuid() == geteuid() && getgid() == getegid()) {
        const char * tmp = getenv(var);
        if (tmp != NULL)
            result = tmp;
    }
    return result;
}

#endif // _LINUX
