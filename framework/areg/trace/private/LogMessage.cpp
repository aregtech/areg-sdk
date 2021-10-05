/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/LogMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
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

LogMessage::LogMessage(NETrace::eLogType logType, const TraceScope & traceScope )
    : NETrace::sLogMessage( logType, traceScope.getScopeId(), NETrace::PrioScope, traceScope.getScopeName().getString(), traceScope.getScopeName( ).getLength() )
{
    ASSERT( (logType == NETrace::LogScopeEnter) || (logType == NETrace::LogScopeExit) );
}

void LogMessage::setMessage(const char * message, int msgLen )
{
    if ( (message != nullptr) && (msgLen > 0) )
    {
        int len = NEMemory::memCopy( this->lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, message, msgLen);
        this->lmTrace.traceMessage[len] = String::EmptyChar;
    }
}
