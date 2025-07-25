/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LogMessage.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       NELogging namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/logging/private/LogMessage.hpp"

#include "areg/logging/LogScope.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Thread.hpp"

#include <string.h>

#if AREG_LOGS
LogMessage::LogMessage(NELogging::eLogMessageType msgType, unsigned int sessionId, const LogScope & logScope )
    : NELogging::sLogMessage( msgType, logScope.getScopeId(), sessionId, NELogging::PrioScope, logScope.getScopeName().getString(), static_cast<uint32_t>(logScope.getScopeName( ).getLength()) )
{
    // AAvetyan: check that the message type is either LogMessageScopeEnter or LogMessageScopeExit
    ASSERT( ((static_cast<uint8_t>(NELogging::eLogMessageType::LogMessageScopeEnter) & static_cast<uint8_t>(msgType)) != 0) || 
            ((static_cast<uint8_t>(NELogging::eLogMessageType::LogMessageScopeExit)  & static_cast<uint8_t>(msgType)) != 0) );
}

void LogMessage::setMessage(const char * message, int msgLen )
{
    uint32_t len = NEMemory::memCopy(this->logMessage, NELogging::LOG_MESSAGE_IZE - 1, message, static_cast<uint32_t>(msgLen));
    this->logMessage[len] = String::EmptyChar;
}

#else   // AREG_LOGS

LogMessage::LogMessage(NELogging::eLogMessageType /*msgType*/, unsigned int /*sessionId*/, const LogScope& /*logScope*/)
    : NELogging::sLogMessage( )
{
}

#endif  // AREG_LOGS
