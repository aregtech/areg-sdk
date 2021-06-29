/************************************************************************
 * \file        areg/trace/private/LogMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

LogMessage::LogMessage(NETrace::eLogType logType, const TraceScope & traceScope )
    : NETrace::sLogMessage( logType, traceScope.getScopeId(), NETrace::PrioScope, traceScope.getScopeName() )
{
    ASSERT(logType == NETrace::LogScopeEnter || logType == NETrace::LogScopeExit);
}

void LogMessage::setMessage(const char * message)
{
    NEString::copyString<char, char>(lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, message);
}
