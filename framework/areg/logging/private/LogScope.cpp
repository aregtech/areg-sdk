/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogScope.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
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

LogScope::LogScope( const char * scopeName, NELogging::LogPriority priority /*= NELogging::PrioNotset*/ )
    : mScopeId      ( NELogging::make_scope_id(scopeName)  )
    , mScopePrio    ( static_cast<uint32_t>(priority) )
    , mScopeName    ( scopeName != nullptr ? scopeName : "" )
    , mIsRegistered ( true )
    , mSessionId    ( 0 )
{
    LogManager::register_log_scope( self() );
}

LogScope::LogScope(const InStream & stream)
    : mScopeId      ( stream.read32_bits() )
    , mScopePrio    ( stream.read32_bits() )
    , mScopeName    ( stream )
    , mIsRegistered ( false )
    , mSessionId    ( 0 )
{
}

LogScope::~LogScope()
{
    if (mIsRegistered)
    {
        LogManager::unregister_log_scope(self());
    }
}

void LogScope::set_priority(const char* newPrio)
{
    set_priority(static_cast<uint32_t>(NELogging::string_to_priority(newPrio)));
}

void LogScope::set_priority(const String& newPrio)
{
    set_priority(static_cast<uint32_t>(NELogging::string_to_priority(newPrio)));
}

#else   // AREG_LOGS

LogScope::LogScope(const char* /*scopeName*/, NELogging::LogPriority /*priority*/ /*= NELogging::PrioNotset*/)
    : mScopeId      ( 0 )
    , mScopePrio    ( static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid) )
    , mScopeName    ( )
    , mIsRegistered (false)
{
}

LogScope::LogScope(const InStream& /*stream*/ )
    : mScopeId      ( 0 )
    , mScopePrio    ( static_cast<uint32_t>(NELogging::LogPriority::PrioInvalid) )
    , mScopeName    ( )
    , mIsRegistered (false)
{
}

LogScope::~LogScope()
{
}

#endif  // AREG_LOGS
