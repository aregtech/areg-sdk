/************************************************************************
 * \file        areg/trace/private/NETrace.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/TraceManager.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/DateTime.hpp"

AREG_API const char * const     NETrace::PRIO_NOTSET_STR       = "NOTSET";
AREG_API const char * const     NETrace::PRIO_FATAL_STR        = "FATAL";
AREG_API const char * const     NETrace::PRIO_ERROR_STR        = "ERROR";
AREG_API const char * const     NETrace::PRIO_WARNING_STR      = "WARN";
AREG_API const char * const     NETrace::PRIO_INFO_STR         = "INFO";
AREG_API const char * const     NETrace::PRIO_DEBUG_STR        = "DEBUG";
AREG_API const char * const     NETrace::PRIO_SCOPE_STR        = "SCOPE";


AREG_API const char * NETrace::convToString( NETrace::eLogPriority prio )
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

AREG_API NETrace::eLogPriority NETrace::convFromString( const char * strPrio )
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
    : traceThreadId     ( Thread::getCurrentThreadId() )
    , traceScopeId      ( NETrace::TRACE_SCOPE_ID_NONE )
    , traceTimestamp    ( DateTime::getNow() )
    , traceMessagePrio  ( NETrace::PrioNotset )
{
    traceMessage[0] = String::EmptyChar;
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
    : traceThreadId     ( Thread::getCurrentThreadId() )
    , traceScopeId      ( scopeId )
    , traceTimestamp    ( DateTime::getNow() )
    , traceMessagePrio  ( msgPrio )
{
    NEString::copyString<char, char>( traceMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE, message != NULL ? message : String::EmptyString );
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

AREG_API bool NETrace::startLogging(const char * fileConfig /*= NULL */)
{
    return TraceManager::startLogging(fileConfig);
}

AREG_API void NETrace::stopLogging(void)
{
    TraceManager::stopLogging();
}

AREG_API void NETrace::activateScope(TraceScope & traceScope)
{
    TraceManager::activateTraceScope(traceScope);
}

AREG_API bool NETrace::isStarted( void )
{
    return TraceManager::isLoggingStarted();
}

AREG_API bool NETrace::isConfigured(void)
{
    return TraceManager::isLoggingEnabled();
}

AREG_API bool NETrace::configureLoging(const char * fileConfig)
{
    return TraceManager::configureLogging(fileConfig);
}

AREG_API bool NETrace::isEnabled(void)
{
    return TraceManager::isLoggingEnabled();
}

AREG_API bool NETrace::forceStartLogging(void)
{
    return TraceManager::forceActivateLogging();
}
