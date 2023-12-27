/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogMessage.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/trace/private/LogMessage.hpp"

#include "areg/trace/TraceScope.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/Thread.hpp"

#include <string.h>

LogMessage::LogMessage(NETrace::eLogMessageType msgType, const TraceScope & traceScope )
    : NETrace::sLogMessage( msgType, traceScope.getScopeId(), NETrace::PrioScope, traceScope.getScopeName().getString(), traceScope.getScopeName( ).getLength() )
{
    ASSERT( (msgType == NETrace::eLogMessageType::LogMessageScopeEnter) || (msgType == NETrace::eLogMessageType::LogMessageScopeExit) );
}

void LogMessage::setMessage(const char * message, int msgLen )
{
    uint32_t len = NEMemory::memCopy(this->logMessage, NETrace::LOG_MESSAGE_IZE - 1, message, msgLen);
    this->logMessage[len] = String::EmptyChar;
}
