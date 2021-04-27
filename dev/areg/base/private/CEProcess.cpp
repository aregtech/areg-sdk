/************************************************************************
 * \file        areg/base/private/CEProcess.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              OS indipendant implementation
 ************************************************************************/
#include "areg/base/CEProcess.hpp"

#include "areg/base/CEFile.hpp"

//////////////////////////////////////////////////////////////////////////
// CEProcess class implementation
//////////////////////////////////////////////////////////////////////////

const ITEM_ID   CEProcess::CURRENT_PROCESS  = static_cast<ITEM_ID>(0);
const ITEM_ID   CEProcess::UNKNOWN_PROCESS  = static_cast<ITEM_ID>(~0);

CEProcess & CEProcess::GetProcess( void )
{
    static CEProcess    _process;
    return _process;
}

CEProcess::~CEProcess( void )
{
    mProcessHandle      = NULL;
    mProcessId          = CEProcess::UNKNOWN_PROCESS;
}

void CEProcess::initPaths( const char * fullPath )
{
    mProcessFullPath= fullPath;


    NEString::CharPos pos = mProcessFullPath.FindLastOf(CEFile::PATH_SEPARATOR);
    if ( pos != NEString::InvalidPos )
    {
        mProcessPath   = mProcessFullPath.Substr(NEString::StartPos, static_cast<NEString::CharCount>(pos));
        mProcessName   = mProcessFullPath.Substr(pos + 1, NEString::EndPos);
        mAppName       = mProcessName;     // initial value
    }

    pos = mProcessName.FindLastOf(CEFile::EXTENSION_SEPARATOR);
    if ( pos != NEString::InvalidPos )
    {
        mAppName    = mProcessName.Substr(NEString::StartPos, static_cast<NEString::CharCount>(pos));
        mProcessExt = mProcessName.Substr(pos + 1, NEString::EndPos);
    }
}
