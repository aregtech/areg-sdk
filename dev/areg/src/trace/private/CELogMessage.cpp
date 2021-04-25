/************************************************************************
 * \file        areg/src/trace/private/CELogMessage.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/trace/private/CELogMessage.hpp"

#include "areg/src/trace/CETraceScope.hpp"
#include "areg/src/base/IEIOStream.hpp"
#include "areg/src/base/CEThread.hpp"

CELogMessage::CELogMessage(NETrace::eLogType logType, const CETraceScope & traceScope )
    : NETrace::sLogMessage( logType, traceScope.GetScopeId(), NETrace::PrioScope, traceScope.GetScopeName() )
{
    ASSERT(logType == NETrace::LogScopeEnter || logType == NETrace::LogScopeExit);
}

void CELogMessage::SetMessage(const char * message)
{
    NEString::copyString<char, char>(lmTrace.traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, message);
}
