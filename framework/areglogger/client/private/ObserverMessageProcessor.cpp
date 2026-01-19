/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/ObserverMessageProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/private/ObserverMessageProcessor.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/base/Process.hpp"
#include "areg/component/NEService.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/logging/LogScope.hpp"

#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/LogObserverBase.hpp"

ObserverMessageProcessor::ObserverMessageProcessor(LoggerClient& loggerClient)
    : mLoggerClient (loggerClient)
{
}

void ObserverMessageProcessor::notifyServiceConnection(const RemoteMessage& msgReceived)
{
    ITEM_ID cookie{ NEService::COOKIE_UNKNOWN };
    NEService::eServiceConnection connection{ NEService::eServiceConnection::ServiceConnectionUnknown };
    msgReceived.moveToBegin();
    msgReceived >> cookie;
    msgReceived >> connection;

    NELogging::sLogMessage log;
    _initLocalLogMessage(log, cookie, 0);
    switch (connection)
    {
    case NEService::eServiceConnection::ServiceConnected:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "Log observer connected to log collector service.");
        break;
    case NEService::eServiceConnection::ServicePending:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "The connection to the log collector service is pending.");
        break;
    case NEService::eServiceConnection::ServiceConnectionLost:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "The connection to the log collector service is lost.");
        break;
    case NEService::eServiceConnection::ServiceDisconnected:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "Log observer disconnected from log collector service.");
        break;
    case NEService::eServiceConnection::ServiceFailed:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "Failed to connect to the log collector service.");
        break;
    case NEService::eServiceConnection::ServiceRejected:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "The connection to the log collector service is rejected.");
        break;
    case NEService::eServiceConnection::ServiceShutdown:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "The log collector service is shutting down.");
        break;
    case NEService::eServiceConnection::ServiceConnectionUnknown:
    default:
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "Undefined log collector service connection event...");
        break;
    }

    RemoteMessage msgLog = NELogging::createLogMessage(log, NELogging::eLogDataType::LogDataLocal, cookie);
    notifyLogMessage(msgLog);
}

void ObserverMessageProcessor::notifyConnectedClients(const RemoteMessage& msgReceived)
{
    NERemoteService::eRemoteConnection remConnect{ NERemoteService::eRemoteConnection::RemoteDisconnected };

    do
    {
        Lock lock(mLoggerClient.mLock);
        msgReceived >> remConnect;

        if (remConnect == NERemoteService::eRemoteConnection::RemoteConnected)
        {
            _clientsConnected(msgReceived);
        }
        else
        {
            _clientsDisconnected(msgReceived);
        }
    } while (false);
}

void ObserverMessageProcessor::notifyLogRegisterScopes(const RemoteMessage& msgReceived)
{
    FuncLogRegisterScopes callback{ nullptr };
    ITEM_ID cookie{ msgReceived.getSource() };
    sLogScope* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::getNow());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogRegisterScopes : nullptr;
        mLoggerClient.mLogDatabase.logScopesDeactivate(cookie, now);
        msgReceived >> count;
        scopes = count != 0 ? new sLogScope[count] : nullptr;
        if (scopes == nullptr)
        {
            count = 0;
            break;
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            LogScope scope(msgReceived);
            sLogScope& entry{ scopes[i] };
            entry.lsId = scope.getScopeId();
            entry.lsPrio = scope.getPriority();
            NEMemory::memCopy(entry.lsName, LENGTH_SCOPE, scope.getScopeName().getString(), scope.getScopeName().getLength() + 1);
            mLoggerClient.mLogDatabase.logScopeActivate(scope.getScopeName(), scope.getScopeId(), scope.getPriority(), cookie, now);
        }

        NELogging::sLogMessage log;
        _initLocalLogMessage(log, NEService::COOKIE_LOGGER, now);
        log.logMessageLen = String::formatString(log.logMessage, NELogging::LOG_MESSAGE_IZE, "Log observer registered %u scopes of instance %lu.", count, static_cast<uint64_t>(cookie));
        RemoteMessage msgLog = NELogging::createLogMessage(log, NELogging::eLogDataType::LogDataLocal, NEService::COOKIE_LOGGER);
        notifyLogMessage(msgLog);

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogRegisterScopes(cookie, scopes, count);
    }
    else if (callback != nullptr)
    {
        callback(cookie, scopes, count);
    }

    if (scopes != nullptr)
    {
        delete[] scopes;
    }
}

void ObserverMessageProcessor::notifyLogUpdateScopes(const RemoteMessage& msgReceived)
{
    FuncLogUpdateScopes callback{ nullptr };
    ITEM_ID cookie{ msgReceived.getSource() };
    sLogScope* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::getNow());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogUpdatedScopes : nullptr;
        mLoggerClient.mLogDatabase.logScopesDeactivate(cookie, now);
        msgReceived >> count;
        scopes = count != 0 ? new sLogScope[count] : nullptr;
        if (scopes == nullptr)
        {
            count = 0;
            break;
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            LogScope scope(msgReceived);
            sLogScope& entry{ scopes[i] };
            entry.lsId = scope.getScopeId();
            entry.lsPrio = scope.getPriority();
            NEMemory::memCopy(entry.lsName, LENGTH_SCOPE, scope.getScopeName().getString(), scope.getScopeName().getLength() + 1);
            mLoggerClient.mLogDatabase.logScopeActivate(scope.getScopeName(), scope.getScopeId(), scope.getPriority(), cookie, now);
        }

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogUpdateScopes(cookie, scopes, count);
    }
    else if (callback != nullptr)
    {
        callback(cookie, scopes, count);
    }

    if (scopes != nullptr)
    {
        delete[] scopes;
    }
}

void ObserverMessageProcessor::notifyLogMessage(const RemoteMessage& msgReceived)
{
    FuncLogMessage callback{ nullptr };
    FuncLogMessageEx callbackEx{ nullptr };
    sLogMessage msgLog{ };
    const unsigned char* logBuffer{ nullptr };
    unsigned int size{ 0 };
    DateTime now{ DateTime::getNow() };

    do
    {
        Lock lock(mLoggerClient.mLock);
        const NELogging::sLogMessage* msgRemote = reinterpret_cast<const NELogging::sLogMessage*>(msgReceived.getBuffer());
        ASSERT(msgRemote != nullptr);
        const_cast<NELogging::sLogMessage*>(msgRemote)->logReceived = static_cast<TIME64>(now);
        if (mLoggerClient.mLogDatabase.logMessage(*msgRemote) == false)
        {
            // If log message is not stored, it is not processed
            break;
        }

        mLoggerClient.mLogDatabase.commit(true);

        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->onLogMessage(msgReceived);
        }
        else if (mLoggerClient.mCallbacks != nullptr)
        {
            if (mLoggerClient.mCallbacks->evtLogMessage != nullptr)
            {
                callback = mLoggerClient.mCallbacks->evtLogMessage;

                msgLog.msgType      = static_cast<eLogType>(msgRemote->logMsgType);
                msgLog.msgPriority  = static_cast<eLogPriority>(msgRemote->logMessagePrio);
                msgLog.msgSource    = static_cast<unsigned long long>(msgRemote->logSource);
                msgLog.msgCookie    = static_cast<unsigned long long>(msgRemote->logCookie);
                msgLog.msgModuleId  = static_cast<unsigned long long>(msgRemote->logModuleId);
                msgLog.msgThreadId  = static_cast<unsigned long long>(msgRemote->logThreadId);
                msgLog.msgTimestamp = static_cast<unsigned long long>(msgRemote->logTimestamp);
                msgLog.msgReceived  = static_cast<unsigned long long>(msgRemote->logReceived);
                msgLog.msgDuration  = static_cast<unsigned int>(msgRemote->logDuration);
                msgLog.msgScopeId   = static_cast<unsigned int>(msgRemote->logScopeId);
                msgLog.msgSessionId = static_cast<unsigned int>(msgRemote->logSessionId);

                NEMemory::memCopy(msgLog.msgLogText, LENGTH_MESSAGE , msgRemote->logMessage , msgRemote->logMessageLen + 1);
                NEMemory::memCopy(msgLog.msgThread,  LENGTH_NAME    , msgRemote->logThread  , msgRemote->logThreadLen  + 1);
                NEMemory::memCopy(msgLog.msgModule,  LENGTH_NAME    , msgRemote->logModule  , msgRemote->logModuleLen  + 1);
            }
            else if (mLoggerClient.mCallbacks->evtLogMessageEx != nullptr)
            {
                callbackEx = mLoggerClient.mCallbacks->evtLogMessageEx;
                logBuffer = msgReceived.getBuffer();
                size = msgReceived.getSizeUsed();
            }
        }
    } while (false);

    if (callback != nullptr)
    {
        callback(&msgLog);
    }
    else if (callbackEx != nullptr)
    {
        callbackEx(logBuffer, size);
    }
}

void ObserverMessageProcessor::_clientsConnected(const RemoteMessage& msgReceived)
{
    TEArrayList< NEService::sServiceConnectedInstance > listConnected;
    msgReceived >> listConnected;

    FuncInstancesConnect callback{ nullptr };
    sLogInstance* listInstances{ nullptr };
    int size{ static_cast<int>(listConnected.getSize()) };
    if (size == 0)
        return;

    do
    {
        Lock lock(mLoggerClient.mLock);
        DateTime now(DateTime::getNow());

        if (LogObserverBase::_theLogObserver != nullptr)
        {
            for (int i = 0; i < size; ++i)
            {
                const NEService::sServiceConnectedInstance& client{ listConnected[static_cast<uint32_t>(i)] };
                auto added = mLoggerClient.mInstances.addIfUnique(client.ciCookie, client, false);
                if (added.second)
                {
                    mLoggerClient.mLogDatabase.logInstanceConnected(client, now);

                    NELogging::sLogMessage log;
                    _initLocalLogMessage(log, NEService::COOKIE_LOGGER, now);
                    log.logMessageLen = String::formatString( log.logMessage
                                                            , NELogging::LOG_MESSAGE_IZE
                                                            , "Log observer have got %u-bit %s (%lu) client connection event, ready to receive logs."
                                                            , static_cast<uint32_t>(client.ciBitness)
                                                            , client.ciInstance.c_str()
                                                            , static_cast<uint64_t>(client.ciCookie));
                    RemoteMessage msgLog = NELogging::createLogMessage(log, NELogging::eLogDataType::LogDataLocal, NEService::COOKIE_LOGGER);
                    notifyLogMessage(msgLog);
                }
            }

            mLoggerClient.mLogDatabase.commit(true);
        }
        else
        {
            callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstConnected : nullptr;
            listInstances = new sLogInstance[size];

            for (int i = 0; i < size; ++i)
            {
                const NEService::sServiceConnectedInstance& client{ listConnected[static_cast<uint32_t>(i)] };
                auto added = mLoggerClient.mInstances.addIfUnique(client.ciCookie, client, false);
                if (added.second)
                {
                    mLoggerClient.mLogDatabase.logInstanceConnected(client, now);

                    NELogging::sLogMessage log;
                    _initLocalLogMessage(log, NEService::COOKIE_LOGGER, now);
                    log.logMessageLen = String::formatString( log.logMessage
                                                            , NELogging::LOG_MESSAGE_IZE
                                                            , "Log observer have got %u-bit %s (%lu) client connection event, starts receiving logs."
                                                            , static_cast<uint32_t>(client.ciBitness)
                                                            , client.ciInstance.c_str()
                                                            , static_cast<uint64_t>(client.ciCookie));
                    RemoteMessage msgLog = NELogging::createLogMessage(log, NELogging::eLogDataType::LogDataLocal, NEService::COOKIE_LOGGER);
                    notifyLogMessage(msgLog);
                }

                if (listInstances != nullptr)
                {
                    sLogInstance& inst{ listInstances[i] };
                    inst.liSource = static_cast<uint32_t>(client.ciSource);
                    inst.liBitness = static_cast<uint32_t>(client.ciBitness);
                    inst.liCookie = client.ciCookie;
                    inst.liTimestamp = client.ciTimestamp;
                    NEMemory::memCopy(inst.liName    , LENGTH_NAME    , client.ciInstance.c_str(), static_cast<int>(client.ciInstance.length()) + 1);
                    NEMemory::memCopy(inst.liLocation, LENGTH_LOCATION, client.ciLocation.c_str(), static_cast<int>(client.ciLocation.length()) + 1);
                }
            }

            mLoggerClient.mLogDatabase.commit(true);
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogInstancesConnect(listConnected.getData());
    }
    else if (callback != nullptr)
    {
        callback(listInstances, size);
    }

    if (listInstances != nullptr)
    {
        delete[] listInstances;
    }
}

void ObserverMessageProcessor::_clientsDisconnected(const RemoteMessage& msgReceived)
{
    TEArrayList<ITEM_ID> listClients;
    TEArrayList< NEService::sServiceConnectedInstance > listDisconnected;

    msgReceived >> listClients;
    FuncInstancesDisconnect callback{ nullptr };
    ITEM_ID* listInstances{ nullptr };
    int size{ static_cast<int>(listClients.getSize()) };
    int count{ 0 };

    do
    {
        Lock lock(mLoggerClient.mLock);

        if (LogObserverBase::_theLogObserver == nullptr)
        {
            callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstDisconnected : nullptr;
        }


        if (size > 0)
        {
            DateTime now(DateTime::getNow());
            listInstances = new ITEM_ID[size];

            for (int i = 0; i < size; ++i)
            {
                const ITEM_ID& client{ listClients[static_cast<uint32_t>(i)] };
                if (mLoggerClient.mInstances.contains(client))
                {
                    const NEService::sServiceConnectedInstance& instance = mLoggerClient.mInstances.getAt(client);
                    listDisconnected.add(instance);
                    if (mLoggerClient.mInstances.removeAt(client))
                    {
                        mLoggerClient.mLogDatabase.logInstanceDisconnected(client, now);
                    }

                    if (listInstances != nullptr)
                    {
                        listInstances[i] = client;
                        ++count;
                    }
                }
            }

            mLoggerClient.mLogDatabase.commit(true);
        }
    } while (false);


    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->onLogInstancesDisconnect(listDisconnected.getData());
    }
    else if (callback != nullptr)
    {
        callback(listInstances, count);
    }

    if (listInstances != nullptr)
    {
        delete[] listInstances;
    }
}

inline void ObserverMessageProcessor::_initLocalLogMessage(NELogging::sLogMessage& log, ITEM_ID cookie, TIME64 timestamp /*= 0*/) const
{
    Process& process = Process::getInstance();
    String instance  = process.getName();
    DateTime now     = DateTime::getNow();

    log.logDataType     = NELogging::eLogDataType::LogDataLocal;
    log.logMsgType      = NELogging::eLogMessageType::LogMessageText;
    log.logMessagePrio  = NELogging::eLogPriority::PrioAny;
    log.logSource       = NEService::SOURCE_LOCAL;
    log.logTarget       = NEService::TARGET_LOCAL;
    log.logCookie       = cookie;
    log.logModuleId     = Process::CURRENT_PROCESS;
    log.logThreadId     = Thread::INVALID_THREAD_ID;
    log.logTimestamp    = timestamp == 0u ? static_cast<TIME64>(now) : timestamp;
    log.logReceived     = static_cast<TIME64>(now);
    log.logDuration     = 0u;
    log.logScopeId      = NELogging::LOG_SCOPE_ID_NONE;
    log.logSessionId    = 0u;
    log.logMessageLen   = 0u;
    log.logMessage[0]   = String::EmptyChar;
    log.logThreadLen    = 0u;
    log.logThread[0]    = String::EmptyChar;
    log.logModuleLen    = 0u;
    log.logModule[0]    = String::EmptyChar;
}
