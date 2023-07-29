/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NETrace.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       NETrace namespace contains , structures and types.
 *
 ************************************************************************/
#include "areg/trace/NETrace.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/TraceScope.hpp"
#include "areg/trace/private/TraceManager.hpp"

#include <string.h>

NETrace::sLogMessageData::sLogMessageData(NETrace::eMessageType msgType /*= NETrace::eMessageType::MsgUndefined*/)
    : dataMsgType      ( msgType                       )
    , dataHostId       ( TraceManager::getCookie()     )
    , dataModuleId     ( TraceManager::getModuleId()   )
    , dataThreadId     ( Thread::getCurrentThreadId()  )
    , dataTimestamp    ( DateTime::getNow()            )
    , dataScopeId      ( NETrace::TRACE_SCOPE_ID_NONE  )
    , dataMessagePrio  ( NETrace::PrioNotset           )
    , dataMessageLen   ( 0                             )
{
    dataMessage[0] = String::EmptyChar;
}

NETrace::sLogMessageData::sLogMessageData(const sLogMessageData& source)
    : dataMsgType      ( source.dataMsgType       )
    , dataHostId       ( source.dataHostId        )
    , dataModuleId     ( source.dataModuleId      )
    , dataThreadId     ( source.dataThreadId      )
    , dataTimestamp    ( source.dataTimestamp     )
    , dataScopeId      ( source.dataScopeId       )
    , dataMessagePrio  ( source.dataMessagePrio   )
    , dataMessageLen   ( source.dataMessageLen    )
{
    int len = NEMemory::memCopy(dataMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, source.dataMessage, source.dataMessageLen);
    dataMessage[len] = String::EmptyChar;
}

NETrace::sLogMessageData::sLogMessageData(NETrace::eMessageType msgType, unsigned int scopeId, NETrace::eLogPriority msgPrio, const char* message, unsigned int msgLen)
    : dataMsgType      ( msgType                       )
    , dataHostId       ( TraceManager::getCookie()     )
    , dataModuleId     ( TraceManager::getModuleId()   )
    , dataThreadId     ( Thread::getCurrentThreadId()  )
    , dataTimestamp    ( DateTime::getNow()            )
    , dataScopeId      ( scopeId                       )
    , dataMessagePrio  ( msgPrio                       )
    , dataMessageLen   ( MACRO_MIN(LOG_MESSAGE_BUFFER_SIZE - 1, msgLen) )
{
    int len = NEMemory::memCopy(dataMessage, NETrace::LOG_MESSAGE_BUFFER_SIZE - 1, message, msgLen);
    dataMessageLen = len;
    dataMessage[len] = String::EmptyChar;
}

NETrace::sLogMessageData& NETrace::sLogMessageData::operator = (const NETrace::sLogMessageData& source)
{
    if (static_cast<NETrace::sLogMessageData*>(this) != &source)
    {
        dataMsgType    = source.dataMsgType;
        dataHostId     = source.dataHostId;
        dataModuleId   = source.dataModuleId;
        dataThreadId   = source.dataThreadId;
        dataTimestamp  = source.dataTimestamp;
        dataScopeId    = source.dataScopeId;
        dataMessagePrio= source.dataMessagePrio;
        dataMessageLen = source.dataMessageLen;

        int len = NEMemory::memCopy(dataMessage, LOG_MESSAGE_BUFFER_SIZE - 1, source.dataMessage, source.dataMessageLen);
        dataMessageLen = len;
        dataMessage[len] = String::EmptyChar;
    }

    return (*this);
}

NETrace::sLogRequestConnectData::sLogRequestConnectData(void)
    : dataCommand   ( NETrace::eLogCommands::LogRequestConnect )
    , dataModuleType( NETrace::eLogModuleType::LogModuleGenerator )
    , dataHost      { }
    , dataModule    { }
{
    String appName{ Application::getApplicationName() };
    String hostName{ Application::getMachineName() };

    uint32_t lenHost = NEMemory::memCopy(dataHost, NETrace::LOG_NAMES_SIZE - 1, hostName.getString(), hostName.getLength());
    uint32_t lenName = NEMemory::memCopy(dataModule, NETrace::LOG_NAMES_SIZE - 1, appName.getString(), appName.getLength());
    dataHost[lenHost]  = String::EmptyChar;
    dataModule[lenName]= String::EmptyChar;
}

NETrace::sLogRequestConnectData::sLogRequestConnectData(const sLogRequestConnectData& src)
    : dataCommand   (NETrace::eLogCommands::LogRequestConnect)
    , dataModuleType(src.dataModuleType)
    , dataHost      ( )
    , dataModule    ( )
{
    NEMemory::memCopy(dataHost, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.dataHost, NETrace::LOG_MESSAGE_BUFFER_SIZE);
    NEMemory::memCopy(dataModule, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.dataModule, NETrace::LOG_MESSAGE_BUFFER_SIZE);
}

NETrace::sLogRequestConnectData& NETrace::sLogRequestConnectData::operator = (const NETrace::sLogRequestConnectData& src)
{
    if (static_cast<const NETrace::sLogRequestConnectData*>(this) != &src)
    {
        dataModuleType  = src.dataModuleType;
        NEMemory::memCopy(dataHost, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.dataHost, NETrace::LOG_MESSAGE_BUFFER_SIZE);
        NEMemory::memCopy(dataModule, NETrace::LOG_MESSAGE_BUFFER_SIZE, src.dataModule, NETrace::LOG_MESSAGE_BUFFER_SIZE);
    }

    return (*this);
}

AREG_API_IMPL const String& NETrace::convToString( NETrace::eLogPriority prio )
{
    switch ( prio )
    {
    case NETrace::eLogPriority::PrioNotset:
        return NETrace::PRIO_NOTSET_STR;
    case NETrace::eLogPriority::PrioFatal:
        return NETrace::PRIO_FATAL_STR;
    case NETrace::eLogPriority::PrioError:
        return NETrace::PRIO_ERROR_STR;
    case NETrace::eLogPriority::PrioWarning:
        return NETrace::PRIO_WARNING_STR;
    case NETrace::eLogPriority::PrioInfo:
        return NETrace::PRIO_INFO_STR;
    case NETrace::eLogPriority::PrioDebug:
        return NETrace::PRIO_DEBUG_STR;
    case NETrace::eLogPriority::PrioScope:
        return NETrace::PRIO_SCOPE_STR;
    case NETrace::eLogPriority::PrioIgnore:
    case NETrace::eLogPriority::PrioIgnoreLayout:
    case NETrace::eLogPriority::PrioAny:
        return NETrace::PRIO_NO_PRIO;
    default:
        ASSERT(false);
        return NETrace::PRIO_NO_PRIO;
    }
}

AREG_API_IMPL NETrace::eLogPriority NETrace::convFromString( const String& strPrio )
{
    if ( strPrio.isEmpty() == false )
    {
        if ( strPrio.compare( NETrace::PRIO_NOTSET_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioNotset;
        else if (strPrio.compare( NETrace::PRIO_SCOPE_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioScope;
        else if (strPrio.compare( NETrace::PRIO_DEBUG_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioDebug;
        else if (strPrio.compare( NETrace::PRIO_INFO_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioInfo;
        else if (strPrio.compare( NETrace::PRIO_WARNING_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioWarning;
        else if (strPrio.compare( NETrace::PRIO_ERROR_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioError;
        else if (strPrio.compare( NETrace::PRIO_FATAL_STR, false ) == NEMath::eCompare::Equal)
            return NETrace::eLogPriority::PrioFatal;
    }

    return NETrace::eLogPriority::PrioNotset;
}

AREG_API_IMPL bool NETrace::startLogging(const char * fileConfig /*= nullptr */ )
{
#if AREG_LOGS
    return TraceManager::startLogging(fileConfig);
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL void NETrace::stopLogging(void)
{
#if AREG_LOGS
    TraceManager::stopLogging();
#endif  // AREG_LOGS
}

AREG_API_IMPL void NETrace::activateScope(TraceScope & traceScope)
{
#if AREG_LOGS
    TraceManager::activateTraceScope(traceScope);
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isStarted( void )
{
#if AREG_LOGS
    return TraceManager::isLoggingStarted();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isConfigured(void)
{
#if AREG_LOGS
    return TraceManager::isLoggingConfigured();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::configureLoging(const char * fileConfig)
{
#if AREG_LOGS
    return TraceManager::configureLogging(fileConfig);
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::isEnabled(void)
{
#if AREG_LOGS
    return TraceManager::isLoggingEnabled();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL const String& NETrace::getConfigFile(void)
{
#if AREG_LOGS
    return TraceManager::getConfigFile();
#else   // !AREG_LOGS
    return String::getEmptyString();
#endif  // AREG_LOGS
}

AREG_API_IMPL unsigned int NETrace::makeScopeId( const char * scopeName )
{
#if AREG_LOGS
    return  NEMath::crc32Calculate( scopeName );
#else   // !AREG_LOGS
    return 0;
#endif  // AREG_LOGS
}

AREG_API_IMPL SharedBuffer NETrace::messageConnectLogService( void )
{
    SharedBuffer result;
    result << NETrace::sLogRequestConnect( );
    return result;
}

AREG_API_IMPL SharedBuffer NETrace::messageDisconnectLogService( )
{
    SharedBuffer result;
    result << NETrace::sLogRequestDisconnect( );
    return result;
}

AREG_API_IMPL SharedBuffer NETrace::messageRegisterScopesStart( )
{
    NETrace::sLogRequestRegisterScopes regScopes;
    regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesSet;
    SharedBuffer result;
    result << regScopes.reqScopeHeader;
    result << regScopes.reqScopeInfo;
    return result;
}

AREG_API_IMPL SharedBuffer NETrace::messageRegisterScopesEnd( )
{
    NETrace::sLogRequestRegisterScopes regScopes;
    regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesNoAction;
    SharedBuffer result;
    result << regScopes.reqScopeHeader;
    result << regScopes.reqScopeInfo;
    return result;
}

AREG_API_IMPL SharedBuffer NETrace::messageRegisterScopes( const TEHashMap<unsigned int, TraceScope *> & scopeList
                                                         , TEHashMap<unsigned int, TraceScope *>::MAPPOS & startAt
                                                         , unsigned int maxEntries )
{
    using POS = TEHashMap<unsigned int, TraceScope *>::MAPPOS;

    POS end = scopeList.invalidPosition( );
    POS pos = startAt == end ? scopeList.firstPosition( ) : startAt;

    NETrace::sLogRequestRegisterScopes regScopes;
    regScopes.reqScopeInfo.dataScopeAction = NETrace::eScopeAction::ScopesAppend;

    SharedBuffer result;
    result << regScopes.reqScopeHeader;
    result << regScopes.reqScopeInfo;

    if ( maxEntries == 0xFFFFFFFF )
    {
        maxEntries = scopeList.getSize( );
    }

    unsigned int i { 0 };
    for ( ; (i < maxEntries) && pos != end; ++ i )
    {
        TraceScopePair tracePair;
        scopeList.getAtPosition( pos, tracePair );
        ASSERT( tracePair.second != nullptr );
        result << (*tracePair.second);
        pos = scopeList.nextPosition( pos );
    }

    startAt = pos;
    int sizeHeader{ sizeof( NETrace::sLogHeader ) };
    int sizeInfo{ sizeof( NETrace::sLogScopeInfo ) };

    NETrace::sLogHeader * logHeader = reinterpret_cast<NETrace::sLogHeader *>(result.getBuffer( ));
    logHeader->hdrDataLen = result.getSizeUsed( ) - sizeHeader;
    NETrace::sLogScopeInfo * scopeInfo = reinterpret_cast<NETrace::sLogScopeInfo *>(result.getBuffer( ) + sizeHeader);
    scopeInfo->dataBufferLen = result.getSizeUsed( ) - (sizeHeader + sizeInfo);
    scopeInfo->dataScopeCount = i;

    return result;
}

AREG_API_IMPL bool NETrace::forceStartLogging(void)
{
#if AREG_LOGS
    return TraceManager::forceActivateLogging();
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL bool NETrace::configAndStart(const char * fileConfig /*= nullptr */)
{
#if AREG_LOGS
    if (TraceManager::configureLogging(fileConfig))
    {
        TraceManager::forceEnableLogging();
        return TraceManager::startLogging(nullptr);
    }
    else
    {
        return TraceManager::forceActivateLogging();
    }
#else   // !AREG_LOGS
    return true;
#endif  // AREG_LOGS
}

AREG_API_IMPL const ITEM_ID& NETrace::getCookie(void)
{
    return TraceManager::getCookie();
}
