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
 * \brief       NELogging namespace contains , structures and types.
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

#if AREG_LOGS
LogMessage::LogMessage(NELogging::LogMessageType msgType, uint32_t sessionId, TIME64 scopeStamp, const LogScope & logScope )
    : NELogging::LogEntry( msgType, logScope.scope_id(), sessionId, scopeStamp, NELogging::LogPriority::PrioScope, logScope.scope_name().as_string(), static_cast<uint32_t>(logScope.scope_name( ).length()) )
{
    // AAvetyan: check that the message type is either ScopeEnter or ScopeExit
    ASSERT( ((static_cast<uint8_t>(NELogging::LogMessageType::ScopeEnter) & static_cast<uint8_t>(msgType)) != 0) || 
            ((static_cast<uint8_t>(NELogging::LogMessageType::ScopeExit)  & static_cast<uint8_t>(msgType)) != 0) );
}

void LogMessage::set_message(const char * message, int32_t msgLen )
{
    uint32_t len = NEMemory::mem_copy(this->log_message, NELogging::LOG_MESSAGE_IZE - 1, message, static_cast<uint32_t>(msgLen));
    this->log_message[len] = String::EmptyChar;
}

#else   // AREG_LOGS

LogMessage::LogMessage(NELogging::LogMessageType /*msgType*/, uint32_t /*sessionId*/, TIME64 scopeStamp, const LogScope& /*logScope*/)
    : NELogging::LogEntry( )
{
}

#endif  // AREG_LOGS
