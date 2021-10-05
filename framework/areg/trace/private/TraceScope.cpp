/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceScope.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
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

TraceScope::TraceScope( const char * scopeName, NETrace::eLogPriority priority /*= NETrace::PrioNotset*/ )
    : mScopeName    ( scopeName != nullptr ? scopeName : "" )
    , mScopeId      ( TraceManager::makeScopeId(mScopeName.getString())  )
    , mScopePrio    ( priority )
{
    TraceManager::getInstance().registerTraceScope( self() );
}

TraceScope::~TraceScope( void )
{
    TraceManager::getInstance().unregisterTraceScope( self() );
}
