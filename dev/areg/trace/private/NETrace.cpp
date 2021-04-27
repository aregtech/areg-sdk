/************************************************************************
 * \file        areg/trace/private/NETrace.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/CETraceManager.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/base/CEDateTime.hpp"

AREG_API const char * const     NETrace::PRIO_NOTSET_STR       = "NOTSET";
AREG_API const char * const     NETrace::PRIO_FATAL_STR        = "FATAL";
AREG_API const char * const     NETrace::PRIO_ERROR_STR        = "ERROR";
AREG_API const char * const     NETrace::PRIO_WARNING_STR      = "WARN";
AREG_API const char * const     NETrace::PRIO_INFO_STR         = "INFO";
AREG_API const char * const     NETrace::PRIO_DEBUG_STR        = "DEBUG";
AREG_API const char * const     NETrace::PRIO_SCOPE_STR        = "SCOPE";


AREG_API const char * NETrace::ToString( NETrace::eLogPriority prio )
{
    switch ( prio )
    {
    case NETrace::PrioNotset:
        return NETrace::PRIO_NOTSET_STR;
    case NETrace::PrioFatal:
        return NETrace::PRIO_FATAL_STR;
    case NETrace::PrioError:
        return NETrace::PRIO_ERROR_STR;
    case NETrace::PrioWarning:
        return NETrace::PRIO_WARNING_STR;
    case NETrace::PrioInfo:
        return NETrace::PRIO_INFO_STR;
    case NETrace::PrioDebug:
        return NETrace::PRIO_DEBUG_STR;
    case NETrace::PrioScope:
        return NETrace::PRIO_SCOPE_STR;
    case NETrace::PrioIgnore:
    case NETrace::PrioIgnoreLayout:
    case NETrace::PrioAny:
        return "";
    default:
        ASSERT(false);
        return "";
    }
}

AREG_API NETrace::eLogPriority NETrace::FromString( const char * strPrio )
{
    if ( strPrio != NULL && *strPrio != '\0' )
    {
        if ( NEString::compareIgnoreCase( strPrio, NETrace::PRIO_NOTSET_STR ) == 0 )
            return NETrace::PrioNotset;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_SCOPE_STR ) == 0 )
            return NETrace::PrioScope;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_DEBUG_STR ) == 0 )
            return NETrace::PrioDebug;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_INFO_STR ) == 0 )
            return NETrace::PrioInfo;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_WARNING_STR ) == 0 )
            return NETrace::PrioWarning;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_ERROR_STR ) == 0 )
            return NETrace::PrioError;
        else if ( NEString::compareIgnoreCase(strPrio, NETrace::PRIO_FATAL_STR ) == 0 )
            return NETrace::PrioFatal;
    }
    return NETrace::PrioNotset;
}


NETrace::S_LogHeader::S_LogHeader( NETrace::eLogType logType /*= NETrace::LogUndefined*/ )
    : logLength     ( sizeof(NETrace::S_LogMessage) )
    , logType       ( logType )
    , logModuleId   ( 0 )
{
    ; // do nothing
}

NETrace::S_LogHeader::S_LogHeader(const NETrace::S_LogHeader & src)
    : logLength     ( src.logLength )
    , logType       ( src.logType )
    , logModuleId   ( src.logModuleId )
{
    ; // do nothing
}

void NETrace::S_LogHeader::operator = (const NETrace::S_LogHeader & src)
{
    if ( static_cast<const NETrace::S_LogHeader *>(this) != &src )
    {
        logLength   = src.logLength;
        logType     = src.logType;
        logModuleId = src.logModuleId;
    }
}

NETrace::S_LogData::S_LogData(void)
    : traceThreadId     ( CEThread::GetCurrentThreadId() )
    , traceScopeId      ( NETrace::TRACE_SCOPE_ID_NONE )
    , traceTimestamp    ( CEDateTime::GetNow() )
    , traceMessagePrio  ( NETrace::PrioNotset )
{
    traceMessage[0] = CEString::EmptyChar;
}

NETrace::S_LogData::S_LogData(const S_LogData & src)
    : traceThreadId     ( src.traceThreadId )
    , traceScopeId      ( src.traceScopeId )
    , traceTimestamp    ( src.traceTimestamp )
    , traceMessagePrio  ( src.traceMessagePrio )
{
    NEString::copyString<char, char>( traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.traceMessage );
}

NETrace::S_LogData::S_LogData(unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message /*= NULL */)
    : traceThreadId     ( CEThread::GetCurrentThreadId() )
    , traceScopeId      ( scopeId )
    , traceTimestamp    ( CEDateTime::GetNow() )
    , traceMessagePrio  ( msgPrio )
{
    NEString::copyString<char, char>( traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, message != NULL ? message : CEString::EmptyString );
}

void NETrace::S_LogData::operator = (const S_LogData & src)
{
    if (static_cast<NETrace::S_LogData *>(this) != &src)
    {
        traceThreadId   = src.traceThreadId;
        traceScopeId    = src.traceScopeId;
        traceTimestamp  = src.traceTimestamp;
        traceMessagePrio= src.traceMessagePrio;
        NEString::copyString<char, char>( traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.traceMessage );
    }
}

NETrace::S_LogMessage::S_LogMessage(void)
    : lmHeader  ( NETrace::LogUndefined )
    , lmTrace   ( )
{
    ; // do nothing
}

NETrace::S_LogMessage::S_LogMessage(const NETrace::S_LogMessage & src)
    : lmHeader  ( src.lmHeader )
    , lmTrace   ( src.lmTrace )
{
    ; // do nothing
}

NETrace::S_LogMessage::S_LogMessage(NETrace::eLogType logType)
    : lmHeader  ( logType )
    , lmTrace   ( )
{
    ; // do nothing
}

NETrace::S_LogMessage::S_LogMessage(NETrace::eLogType logType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char * message /*= NULL */)
    : lmHeader  ( logType )
    , lmTrace   ( scopeId, msgPrio, message )
{
    ; // do nothing
}


void NETrace::S_LogMessage::operator = (const S_LogMessage & src)
{
    if ( static_cast<const NETrace::S_LogMessage *>(this) != &src )
    {
        lmHeader= src.lmHeader;
        lmTrace = src.lmTrace;
    }
}

AREG_API bool NETrace::StartLogging(const char * fileConfig /*= NULL */)
{
    return CETraceManager::StartLogging(fileConfig);
}

AREG_API void NETrace::StopLogging(void)
{
    CETraceManager::StopLogging();
}

AREG_API void NETrace::ActivateScope(CETraceScope & traceScope)
{
    CETraceManager::ActivateTraceScope(traceScope);
}

AREG_API bool NETrace::IsStarted( void )
{
    return CETraceManager::IsLoggingStarted();
}

AREG_API bool NETrace::IsConfigured(void)
{
    return CETraceManager::IsLoggingEnabled();
}

AREG_API bool NETrace::LogingConfigure(const char * fileConfig)
{
    return CETraceManager::LoggingConfigure(fileConfig);
}

AREG_API bool NETrace::IsEnabled(void)
{
    return CETraceManager::IsLoggingEnabled();
}

AREG_API bool NETrace::ForceStartLogging(void)
{
    return CETraceManager::ForceActivateLogging();
}
