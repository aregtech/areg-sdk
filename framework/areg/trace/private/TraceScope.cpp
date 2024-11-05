/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceScope.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Trace messaging scope object definition.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"

//////////////////////////////////////////////////////////////////////////////
// TraceScope class implementation
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////

#if AREG_LOGS

TraceScope::TraceScope( const char * scopeName, NETrace::eLogPriority priority /*= NETrace::PrioNotset*/ )
    : mScopeName    ( scopeName != nullptr ? scopeName : "" )
    , mScopeId      ( NETrace::makeScopeId(mScopeName.getString())  )
    , mScopePrio    ( priority )
{
    TraceManager::registerTraceScope( self() );
}

TraceScope::TraceScope(const IEInStream & stream)
    : mScopeName    (stream)
    , mScopeId      (stream.read32Bits())
    , mScopePrio    (stream.read32Bits())
{
}

TraceScope::~TraceScope( void )
{
    TraceManager::unregisterTraceScope( self() );
}

void TraceScope::setPriority(const char* newPrio)
{
    setPriority(NETrace::stringToLogPrio(newPrio));
}

void TraceScope::setPriority(const String& newPrio)
{
    setPriority(NETrace::stringToLogPrio(newPrio));
}

#else   // AREG_LOGS

TraceScope::TraceScope(const char* /*scopeName*/, NETrace::eLogPriority /*priority*/ /*= NETrace::PrioNotset*/)
    : mScopeName    ()
    , mScopeId      ( 0 )
    , mScopePrio    ( static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid) )
{
}

TraceScope::TraceScope(const IEInStream& /*stream*/ )
    : mScopeName    ()
    , mScopeId      ( 0 )
    , mScopePrio    ( static_cast<unsigned int>(NETrace::eLogPriority::PrioInvalid) )
{
}

TraceScope::~TraceScope(void)
{
}

#endif  // AREG_LOGS
