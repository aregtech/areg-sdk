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

namespace areg
{

    //////////////////////////////////////////////////////////////////////////////
    // LogScope class implementation
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////////

    #if AREG_LOGS

    LogScope::LogScope( const char * scopeName, areg::LogPriority priority /*= areg::PrioNotset*/ )
        : mScopeId      ( areg::makeScopeId(scopeName)  )
        , mScopePrio    ( static_cast<uint32_t>(priority) )
        , mScopeName    ( scopeName != nullptr ? scopeName : "" )
        , mIsRegistered ( true )
        , mSessionId    ( 0 )
    {
        areg::LogManager::registerLogScope( self() );
    }

    LogScope::LogScope(const areg::InStream & stream)
        : mScopeId      ( stream.read32Bits() )
        , mScopePrio    ( stream.read32Bits() )
        , mScopeName    ( stream )
        , mIsRegistered ( false )
        , mSessionId    ( 0 )
    {
    }

    LogScope::~LogScope()
    {
        if (mIsRegistered)
        {
            areg::LogManager::unregisterLogScope(self());
        }
    }

    void LogScope::setPriority(const char* newPrio)
    {
        setPriority(static_cast<uint32_t>(areg::stringToLogPrio(newPrio)));
    }

    void LogScope::setPriority(const areg::String& newPrio)
    {
        setPriority(static_cast<uint32_t>(areg::stringToLogPrio(newPrio)));
    }

    #else   // AREG_LOGS

    LogScope::LogScope(const char* /*scopeName*/, areg::LogPriority /*priority*/ /*= areg::PrioNotset*/)
        : mScopeId      ( 0 )
        , mScopePrio    ( static_cast<uint32_t>(areg::LogPriority::PrioInvalid) )
        , mScopeName    ( )
        , mIsRegistered (false)
    {
    }

    LogScope::LogScope(const areg::InStream& /*stream*/ )
        : mScopeId      ( 0 )
        , mScopePrio    ( static_cast<uint32_t>(areg::LogPriority::PrioInvalid) )
        , mScopeName    ( )
        , mIsRegistered (false)
    {
    }

    LogScope::~LogScope()
    {
    }

    #endif  // AREG_LOGS
} // namespace areg
