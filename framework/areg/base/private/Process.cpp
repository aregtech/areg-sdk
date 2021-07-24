/************************************************************************
 * \file        areg/base/private/Process.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              OS indipendant implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#include "areg/base/File.hpp"

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

const ITEM_ID   Process::CURRENT_PROCESS  = static_cast<ITEM_ID>(0);
const ITEM_ID   Process::UNKNOWN_PROCESS  = static_cast<ITEM_ID>(~0);

Process Process::_theProcess;

Process::Process( void )
    : mProcEnv          ( static_cast<Process::eProcEnv>(sizeof(id_type)) )
	, mProcessId        ( Process::UNKNOWN_PROCESS )
    , mProcessHandle    ( NULL )
    , mAppName          ( )
    , mProcessName      ( )
    , mProcessExt       ( )
    , mProcessPath      ( )
    , mProcessFullPath  ( )
	, mIsInitialized	( false )
{
}

Process::~Process( void )
{
    mProcessHandle      = NULL;
    mProcessId          = Process::UNKNOWN_PROCESS;
    mIsInitialized		= false;
}

void Process::_initPaths( const char * fullPath )
{
    mProcessFullPath= fullPath;


    NEString::CharPos pos = mProcessFullPath.findLastOf(File::PATH_SEPARATOR);
    if ( pos != NEString::InvalidPos )
    {
        mProcessPath   = mProcessFullPath.substring(NEString::StartPos, static_cast<NEString::CharCount>(pos));
        mProcessName   = mProcessFullPath.substring(pos + 1, NEString::EndPos);
        mAppName       = mProcessName;     // initial value
    }

    pos = mProcessName.findLastOf(File::EXTENSION_SEPARATOR);
    if ( pos != NEString::InvalidPos )
    {
        mAppName    = mProcessName.substring(NEString::StartPos, static_cast<NEString::CharCount>(pos));
        mProcessExt = mProcessName.substring(pos + 1, NEString::EndPos);
    }
}
