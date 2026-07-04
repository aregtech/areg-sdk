/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areglogger/client/private/ObserverMessageProcessor.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/private/ObserverMessageProcessor.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/ArrayList.hpp"
#include "areg/base/Process.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/LogObserverBase.hpp"

namespace areg::logger {

ObserverMessageProcessor::ObserverMessageProcessor(LoggerClient& loggerClient)
    : mLoggerClient (loggerClient)
{
}

void ObserverMessageProcessor::notify_service_connection(const areg::MessageEnvelope& msgReceived)
{
    ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
    areg::ServiceConnectionState connection{ areg::ServiceConnectionState::Unknown };
    msgReceived.move_to_begin();
    msgReceived >> cookie;
    msgReceived >> connection;

    areg::LogEntry log;
    _init_local_log_message(log, cookie, 0);
    switch (connection)
    {
    case areg::ServiceConnectionState::Connected:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "Log observer connected to log collector service."));
        break;
    case areg::ServiceConnectionState::Pending:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "The connection to the log collector service is pending."));
        break;
    case areg::ServiceConnectionState::ConnectionLost:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "The connection to the log collector service is lost."));
        break;
    case areg::ServiceConnectionState::Disconnected:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "Log observer disconnected from log collector service."));
        break;
    case areg::ServiceConnectionState::Failed:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "Failed to connect to the log collector service."));
        break;
    case areg::ServiceConnectionState::Rejected:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "The connection to the log collector service is rejected."));
        break;
    case areg::ServiceConnectionState::Shutdown:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "The log collector service is shutting down."));
        break;
    case areg::ServiceConnectionState::Unknown:
    default:
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage, areg::LOG_MSG_SIZE, "Undefined log collector service connection event..."));
        break;
    }

    areg::MessageEnvelope msgLog = areg::create_log_message(log, areg::LogDataType::Local, cookie);
    notify_log_message(msgLog);
}

void ObserverMessageProcessor::notify_connected_clients(const areg::MessageEnvelope& msgReceived)
{
    areg::RemoteConnectionState remConnect{ areg::RemoteConnectionState::Disconnected };

    do
    {
        Lock lock(mLoggerClient.mLock);
        msgReceived >> remConnect;

        if (remConnect == areg::RemoteConnectionState::Connected)
        {
            _clients_connected(msgReceived);
        }
        else
        {
            _clients_disconnected(msgReceived);
        }
    } while (false);
}

void ObserverMessageProcessor::notify_log_register_scopes(const areg::MessageEnvelope& msgReceived)
{
    FuncLogRegisterScopes callback{ nullptr };
    ITEM_ID cookie{ msgReceived.source() };
    ScopeInfo* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::now());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogRegisterScopes : nullptr;
        mLoggerClient.mLogDatabase.log_scopes_deactivate(cookie, now);
        msgReceived >> count;
        scopes = count != 0 ? new ScopeInfo[count] : nullptr;
        if (scopes == nullptr)
        {
            count = 0;
            break;
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            const uint32_t scopeId   { msgReceived.read32_bits() };
            const uint32_t scopePrio { msgReceived.read32_bits() };
            String         scopeName (msgReceived);
            ScopeInfo &    entry     { scopes[i] };
            entry.lsId   = scopeId;
            entry.lsPrio = scopePrio;
            areg::mem_copy(entry.lsName, LENGTH_SCOPE, scopeName.as_string(), static_cast<uint32_t>(scopeName.length()) + 1u);
            mLoggerClient.mLogDatabase.log_scope_activate(scopeName, scopeId, scopePrio, cookie, now);
        }

        areg::LogEntry log;
        _init_local_log_message(log, areg::COOKIE_LOGGER, now);
        log.logMessageLen = static_cast<uint32_t>(String::format_string(log.logMessage
                                                    , areg::LOG_MSG_SIZE
                                                    , "Log observer registered %u scopes of instance %u."
                                                    , count
                                                    , cookie));
        areg::MessageEnvelope msgLog = areg::create_log_message(log, areg::LogDataType::Local, areg::COOKIE_LOGGER);
        notify_log_message(msgLog);

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_register_scopes(cookie, scopes, static_cast<int32_t>(count));
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

void ObserverMessageProcessor::notify_log_update_scopes(const areg::MessageEnvelope& msgReceived)
{
    FuncLogUpdateScopes callback{ nullptr };
    ITEM_ID cookie{ msgReceived.source() };
    ScopeInfo* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::now());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogUpdatedScopes : nullptr;
        mLoggerClient.mLogDatabase.log_scopes_deactivate(cookie, now);
        msgReceived >> count;
        scopes = count != 0 ? new ScopeInfo[count] : nullptr;
        if (scopes == nullptr)
        {
            count = 0;
            break;
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            const uint32_t scopeId   { msgReceived.read32_bits() };
            const uint32_t scopePrio { msgReceived.read32_bits() };
            String         scopeName (msgReceived);
            ScopeInfo &    entry     { scopes[i] };
            entry.lsId   = scopeId;
            entry.lsPrio = scopePrio;
            areg::mem_copy(entry.lsName, LENGTH_SCOPE, scopeName.as_string(), static_cast<uint32_t>(scopeName.length()) + 1u);
            mLoggerClient.mLogDatabase.log_scope_activate(scopeName, scopeId, scopePrio, cookie, now);
        }

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_update_scopes(cookie, scopes, static_cast<int32_t>(count));
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

void ObserverMessageProcessor::notify_log_message(const areg::MessageEnvelope& msgReceived)
{
    FuncLogMessage callback{ nullptr };
    FuncLogMessageEx callbackEx{ nullptr };
    LogRecord msgLog{ };
    const uint8_t* logBuffer{ nullptr };
    uint32_t size{ 0 };
    DateTime now{ DateTime::now() };

    do
    {
        Lock lock(mLoggerClient.mLock);
        const areg::LogEntry* msgRemote = reinterpret_cast<const areg::LogEntry*>(msgReceived.buffer());
        ASSERT(msgRemote != nullptr);
        const_cast<areg::LogEntry*>(msgRemote)->logReceived = static_cast<TIME64>(now);
        if (mLoggerClient.mLogDatabase.log_message(*msgRemote))
        {
            mLoggerClient.mLogDatabase.commit(true);
        }

        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->on_log_message(msgReceived);
        }
        else if (mLoggerClient.mCallbacks != nullptr)
        {
            if (mLoggerClient.mCallbacks->evtLogMessage != nullptr)
            {
                callback = mLoggerClient.mCallbacks->evtLogMessage;

                msgLog.msgType      = static_cast<LogType>(msgRemote->logMsgType);
                msgLog.msgPriority  = static_cast<::LogPriority>(msgRemote->logMessagePrio);
                msgLog.msgSource    = static_cast<uint64_t>(msgRemote->logSource);
                msgLog.msgCookie    = static_cast<uint64_t>(msgRemote->logCookie);
                msgLog.msgModuleId  = static_cast<uint64_t>(msgRemote->logModuleId);
                msgLog.msgThreadId  = static_cast<uint64_t>(msgRemote->logThreadId);
                msgLog.msgTimestamp = static_cast<uint64_t>(msgRemote->logTimestamp);
                msgLog.msgReceived  = static_cast<uint64_t>(msgRemote->logReceived);
                msgLog.msgDuration  = static_cast<uint32_t>(msgRemote->logDuration);
                msgLog.msgScopeId   = static_cast<uint32_t>(msgRemote->logScopeId);
                msgLog.msgSessionId = static_cast<uint32_t>(msgRemote->logSessionId);

                areg::mem_copy(msgLog.msgLogText, LENGTH_MESSAGE , msgRemote->logMessage , msgRemote->logMessageLen + 1);
                areg::mem_copy(msgLog.msgThread,  LENGTH_NAME    , msgRemote->logThread  , msgRemote->logThreadLen  + 1);
                areg::mem_copy(msgLog.msgModule,  LENGTH_NAME    , msgRemote->logModule  , msgRemote->logModuleLen  + 1);
            }
            else if (mLoggerClient.mCallbacks->evtLogMessageEx != nullptr)
            {
                callbackEx = mLoggerClient.mCallbacks->evtLogMessageEx;
                logBuffer = msgReceived.buffer();
                size = msgReceived.size_used();
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

void ObserverMessageProcessor::_clients_connected(const areg::MessageEnvelope& msgReceived)
{
    ArrayList< areg::ConnectedInstance > listConnected;
    msgReceived >> listConnected;

    FuncInstancesConnect callback{ nullptr };
    LogInstance* listInstances{ nullptr };
    int32_t size{ static_cast<int32_t>(listConnected.size()) };
    if (size == 0)
        return;

    do
    {
        Lock lock(mLoggerClient.mLock);
        DateTime now(DateTime::now());

        if (LogObserverBase::_theLogObserver != nullptr)
        {
            for (int i = 0; i < size; ++i)
            {
                const areg::ConnectedInstance& client{ listConnected[static_cast<uint32_t>(i)] };
                auto added = mLoggerClient.mInstances.add_if_unique(client.ciCookie, client, false);
                if (added.second)
                {
                    mLoggerClient.mLogDatabase.log_instance_connected(client, now);

                    areg::LogEntry log;
                    _init_local_log_message(log, areg::COOKIE_LOGGER, now);
                    log.logMessageLen = static_cast<uint32_t>(String::format_string( log.logMessage
                                                            , static_cast<int32_t>(areg::LOG_MSG_SIZE)
                                                            , "Log observer have got %u-bit %s (%u) client connection event, ready to receive logs."
                                                            , static_cast<uint32_t>(client.ciBitness)
                                                            , client.ciInstance.c_str()
                                                            , client.ciCookie));
                    areg::MessageEnvelope msgLog = areg::create_log_message(log, areg::LogDataType::Local, areg::COOKIE_LOGGER);
                    notify_log_message(msgLog);
                }
            }

            mLoggerClient.mLogDatabase.commit(true);
        }
        else
        {
            callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstConnected : nullptr;
            listInstances = new LogInstance[static_cast<size_t>(size)];

            for (int i = 0; i < size; ++i)
            {
                const areg::ConnectedInstance& client{ listConnected[static_cast<uint32_t>(i)] };
                auto added = mLoggerClient.mInstances.add_if_unique(client.ciCookie, client, false);
                if (added.second)
                {
                    mLoggerClient.mLogDatabase.log_instance_connected(client, now);

                    areg::LogEntry log;
                    _init_local_log_message(log, areg::COOKIE_LOGGER, now);
                    log.logMessageLen = static_cast<uint32_t>(String::format_string( log.logMessage
                                                             , static_cast<int32_t>(areg::LOG_MSG_SIZE)
                                                             , "Log observer have got %u-bit %s (%u) client connection event, starts receiving logs."
                                                             , static_cast<uint32_t>(client.ciBitness)
                                                             , client.ciInstance.c_str()
                                                             , client.ciCookie));
                    areg::MessageEnvelope msgLog = areg::create_log_message(log, areg::LogDataType::Local, areg::COOKIE_LOGGER);
                    notify_log_message(msgLog);
                }

                if (listInstances != nullptr)
                {
                    LogInstance& inst{ listInstances[i] };
                    inst.liSource = static_cast<uint32_t>(client.ciSource);
                    inst.liBitness = static_cast<uint32_t>(client.ciBitness);
                    inst.liCookie = client.ciCookie;
                    inst.liTimestamp = client.ciTimestamp;
                    areg::mem_copy(inst.liName    , static_cast<uint32_t>(LENGTH_NAME)    , client.ciInstance.c_str(), static_cast<uint32_t>(client.ciInstance.length()) + 1);
                    areg::mem_copy(inst.liLocation, static_cast<uint32_t>(LENGTH_LOCATION), client.ciLocation.c_str(), static_cast<uint32_t>(client.ciLocation.length()) + 1);
                }
            }

            mLoggerClient.mLogDatabase.commit(true);
        }
    } while (false);

    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_instances_connect(listConnected.data());
    }
    else if (callback != nullptr)
    {
        callback(listInstances, static_cast<uint32_t>(size));
    }

    if (listInstances != nullptr)
    {
        delete[] listInstances;
    }
}

void ObserverMessageProcessor::_clients_disconnected(const areg::MessageEnvelope& msgReceived)
{
    ArrayList<ITEM_ID> listClients;
    ArrayList< areg::ConnectedInstance > listDisconnected;

    msgReceived >> listClients;
    FuncInstancesDisconnect callback{ nullptr };
    ITEM_ID* listInstances{ nullptr };
    int32_t size{ static_cast<int32_t>(listClients.size()) };
    int32_t count{ 0 };

    do
    {
        Lock lock(mLoggerClient.mLock);

        if (LogObserverBase::_theLogObserver == nullptr)
        {
            callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstDisconnected : nullptr;
        }


        if (size > 0)
        {
            DateTime now(DateTime::now());
            listInstances = new ITEM_ID[static_cast<size_t>(size)];

            for (int i = 0; i < size; ++i)
            {
                const ITEM_ID& client{ listClients[static_cast<uint32_t>(i)] };
                if (mLoggerClient.mInstances.contains(client))
                {
                    const areg::ConnectedInstance& instance = mLoggerClient.mInstances.value_at(client);
                    listDisconnected.add(instance);
                    if (mLoggerClient.mInstances.remove_at(client))
                    {
                        mLoggerClient.mLogDatabase.log_instance_disconnected(client, now);
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
        LogObserverBase::_theLogObserver->on_log_instances_disconnect(listDisconnected.data());
    }
    else if (callback != nullptr)
    {
        callback(listInstances, static_cast<uint32_t>(count));
    }

    if (listInstances != nullptr)
    {
        delete[] listInstances;
    }
}

inline void ObserverMessageProcessor::_init_local_log_message(areg::LogEntry& log, ITEM_ID cookie, TIME64 timestamp /*= 0*/) const
{
    Process& process = Process::instance();
    String instance  = process.name();
    DateTime now     = DateTime::now();

    log.logDataType     = areg::LogDataType::Local;
    log.logMsgType      = areg::LogMessageType::MessageText;
    log.logMessagePrio  = areg::LogPriority::PrioAny;
    log.logSource       = areg::SOURCE_LOCAL;
    log.logTarget       = areg::TARGET_LOCAL;
    log.logCookie       = cookie;
    log.logModuleId     = Process::CURRENT_PROCESS;
    log.logThreadId     = Thread::INVALID_THREAD_ID;
    log.logTimestamp    = timestamp == 0u ? static_cast<TIME64>(now) : timestamp;
    log.logReceived     = static_cast<TIME64>(now);
    log.logDuration     = 0u;
    log.logScopeId      = areg::LOG_SCOPE_ID_NONE;
    log.logSessionId    = 0u;
    log.logMessageLen   = 0u;
    log.logMessage[0]   = String::EmptyChar;
    log.logThreadLen    = 0u;
    log.logThread[0]    = String::EmptyChar;
    log.logModuleLen    = 0u;
    log.logModule[0]    = String::EmptyChar;
}

} // namespace areg::logger
