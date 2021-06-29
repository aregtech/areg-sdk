/************************************************************************
 * \file        areg/trace/private/TraceScope.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Trace messaging scope object definition.
 ************************************************************************/

#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"

//////////////////////////////////////////////////////////////////////////////
// TraceScope class implementation
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////

TraceScope::TraceScope( const char * scopeName, NETrace::eLogPriority priority /*= NETrace::PrioNotset*/ )
    : mScopeName    ( scopeName != NULL ? scopeName : "" )
    , mScopeId      ( TraceManager::makeScopeId(mScopeName.getString())  )
    , mScopePrio    ( priority )
{
    TraceManager::getInstance().registerTraceScope( self() );
}

TraceScope::~TraceScope( void )
{
    TraceManager::getInstance().unregisterTraceScope( self() );
}
