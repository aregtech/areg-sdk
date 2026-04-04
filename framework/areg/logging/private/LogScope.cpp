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

namespace areg {

#if AREG_LOGGING

LogScope::~LogScope()
{
    LogManager::unregister_log_scope( self() );
}

void LogScope::_register() noexcept
{
    LogManager::register_log_scope( self() );
}

void LogScope::set_priority(const char* newPrio) noexcept
{
    set_priority(static_cast<uint32_t>(areg::string_to_priority(newPrio)));
}

void LogScope::set_priority(const areg::String& newPrio) noexcept
{
    set_priority(static_cast<uint32_t>(areg::string_to_priority(newPrio)));
}

#else   // AREG_LOGGING

LogScope::~LogScope()
{
}

void LogScope::_register() noexcept
{
}

#endif  // AREG_LOGGING

} // namespace areg
