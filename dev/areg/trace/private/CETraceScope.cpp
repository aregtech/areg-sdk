/************************************************************************
 * \file        areg/trace/private/CETraceScope.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace messaging scope object definition.
 ************************************************************************/

#include "areg/trace/CETraceScope.hpp"
#include "areg/trace/private/CETraceManager.hpp"

//////////////////////////////////////////////////////////////////////////////
// CETraceScope class implementation
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////

CETraceScope::CETraceScope( const char * scopeName, NETrace::eLogPriority priority /*= NETrace::PrioNotset*/ )
    : mScopeName    ( scopeName != NULL ? scopeName : "" )
    , mScopeId      ( CETraceManager::MakeScopeId(mScopeName.GetBuffer())  )
    , mScopePrio    ( priority )
{
    CETraceManager::GetTraceManager().RegisterTraceScope( self() );
}

CETraceScope::~CETraceScope( void )
{
    CETraceManager::GetTraceManager().UnregisterTraceScope( self() );
}
