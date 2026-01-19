/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogScope.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Log Message object definition.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/LogScope.hpp"
#include "areg/logging/private/LogManager.hpp"

//////////////////////////////////////////////////////////////////////////////
// LogScope class implementation
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////

#if AREG_LOGS

LogScope::LogScope( const char * scopeName, NELogging::eLogPriority priority /*= NELogging::PrioNotset*/ )
    : mScopeId      ( NELogging::makeScopeId(scopeName)  )
    , mScopePrio    ( priority )
    , mScopeName    ( scopeName != nullptr ? scopeName : "" )
    , mIsRegistered ( true )
    , mSessionId    ( 0 )
{
    LogManager::registerLogScope( self() );
}

LogScope::LogScope(const IEInStream & stream)
    : mScopeId      ( stream.read32Bits() )
    , mScopePrio    ( stream.read32Bits() )
    , mScopeName    ( stream )
    , mIsRegistered ( false )
    , mSessionId    ( 0 )
{
}

LogScope::~LogScope( void )
{
    if (mIsRegistered)
    {
        LogManager::unregisterLogScope(self());
    }
}

void LogScope::setPriority(const char* newPrio)
{
    setPriority(NELogging::stringToLogPrio(newPrio));
}

void LogScope::setPriority(const String& newPrio)
{
    setPriority(NELogging::stringToLogPrio(newPrio));
}

#else   // AREG_LOGS

LogScope::LogScope(const char* /*scopeName*/, NELogging::eLogPriority /*priority*/ /*= NELogging::PrioNotset*/)
    : mScopeId      ( 0 )
    , mScopePrio    ( static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid) )
    , mScopeName    ( )
    , mIsRegistered (false)
{
}

LogScope::LogScope(const IEInStream& /*stream*/ )
    : mScopeId      ( 0 )
    , mScopePrio    ( static_cast<unsigned int>(NELogging::eLogPriority::PrioInvalid) )
    , mScopeName    ( )
    , mIsRegistered (false)
{
}

LogScope::~LogScope(void)
{
}

#endif  // AREG_LOGS
