/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogMessage.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       areg namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/logging/private/LogMessage.hpp"

#include "areg/logging/LogScope.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/Thread.hpp"

#include <string.h>

#if AREG_LOGGING

areg::LogMessage::LogMessage(areg::LogMessageType msgType, uint32_t sessionId, TIME64 scopeStamp, const areg::LogScope & logScope ) noexcept
    : areg::LogEntry( msgType, logScope.scope_id(), sessionId, scopeStamp, areg::LogPriority::PrioScope, logScope.scope_name().as_string(), static_cast<uint32_t>(logScope.scope_name( ).length()) )
{
    // AAvetyan: check that the message type is either ScopeEnter or ScopeExit
    ASSERT( ((static_cast<uint8_t>(areg::LogMessageType::ScopeEnter) & static_cast<uint8_t>(msgType)) != 0) || 
            ((static_cast<uint8_t>(areg::LogMessageType::ScopeExit)  & static_cast<uint8_t>(msgType)) != 0) );
}

void areg::LogMessage::set_message(const char * message, int32_t msgLen ) noexcept
{
    uint32_t len = areg::mem_copy(this->logMessage, areg::LOG_MSG_SIZE - 1, message, static_cast<uint32_t>(msgLen));
    this->logMessage[len] = String::EmptyChar;
}

#else   // AREG_LOGGING

areg::LogMessage::LogMessage(areg::LogMessageType /*msgType*/, uint32_t /*sessionId*/, TIME64 scopeStamp, const LogScope& /*logScope*/) noexcept
    : areg::LogEntry( )
{
}

#endif  // AREG_LOGGING
