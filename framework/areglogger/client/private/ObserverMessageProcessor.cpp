/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
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

namespace {
    //! The database ID of the log message delivered on this thread, 0 outside a delivery.
    thread_local uint32_t _deliveredLogId{ 0u };
}

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

    const char * text{ nullptr };
    switch (connection)
    {
    case areg::ServiceConnectionState::Connected:
        text = "Log observer connected to log collector service.";
        break;
    case areg::ServiceConnectionState::Pending:
        text = "The connection to the log collector service is pending.";
        break;
    case areg::ServiceConnectionState::ConnectionLost:
        text = "The connection to the log collector service is lost.";
        break;
    case areg::ServiceConnectionState::Disconnected:
        text = "Log observer disconnected from log collector service.";
        break;
    case areg::ServiceConnectionState::Failed:
        text = "Failed to connect to the log collector service.";
        break;
    case areg::ServiceConnectionState::Rejected:
        text = "The connection to the log collector service is rejected.";
        break;
    case areg::ServiceConnectionState::Shutdown:
        text = "The log collector service is shutting down.";
        break;
    case areg::ServiceConnectionState::Unknown:
    default:
        text = "Undefined log collector service connection event...";
        break;
    }

    add_local_log(cookie, areg::LogPriority::PrioAny, 0, text);
}

void ObserverMessageProcessor::notify_connected_clients(const areg::MessageEnvelope& msgReceived)
{
    areg::RemoteConnectionState remConnect{ areg::RemoteConnectionState::Disconnected };
    msgReceived >> remConnect;

    if (remConnect == areg::RemoteConnectionState::Connected)
    {
        _clients_connected(msgReceived);
    }
    else
    {
        _clients_disconnected(msgReceived);
    }
}
void ObserverMessageProcessor::notify_log_register_scopes(const areg::MessageEnvelope& msgReceived)
{
    FuncLogRegisterScopes callback{ nullptr };
    const ITEM_ID cookie{ msgReceived.source() };
    const DateTime now(DateTime::now());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogRegisterScopes : nullptr;
    } while (false);

    ScopeInfo* scopes{ nullptr };
    const uint32_t count{ _save_scopes(msgReceived, cookie, now, scopes) };
    if (count != 0u)
    {
        String text;
        text.format("Log observer registered %u scopes of instance %u.", count, cookie);
        add_local_log(areg::COOKIE_LOGGER, areg::LogPriority::PrioAny, now, text.as_string());
    }

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
void ObserverMessageProcessor::notify_log_source_state(const areg::MessageEnvelope& msgReceived)
{
    FuncLogSourceState callback{ nullptr };
    ITEM_ID source    { areg::COOKIE_UNKNOWN };
    uint8_t state     { static_cast<uint8_t>(areg::LogSourceState::Undefined) };
    ITEM_ID byObserver{ areg::COOKIE_UNKNOWN };

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogSourceState : nullptr;
    } while (false);

    msgReceived >> source;
    msgReceived >> state;
    msgReceived >> byObserver;

    // The byte arrives over the network, so an unknown state never reaches the observer.
    const areg::LogSourceState srcState{ static_cast<areg::LogSourceState>(state) };
    if ((source != areg::COOKIE_UNKNOWN) && areg::is_source_state_valid(srcState))
    {
        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->on_log_source_state(source, srcState, byObserver);
        }
        else if (callback != nullptr)
        {
            callback(source, state, byObserver);
        }
    }
}

void ObserverMessageProcessor::notify_log_configuration_restored(const areg::MessageEnvelope& msgReceived)
{
    FuncLogConfigRestored callback{ nullptr };
    ITEM_ID cookie{ msgReceived.source() };

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogConfigRestored : nullptr;
    } while (false);

    if (cookie != areg::COOKIE_UNKNOWN)
    {
        if (LogObserverBase::_theLogObserver != nullptr)
        {
            LogObserverBase::_theLogObserver->on_log_configuration_restored(cookie);
        }
        else if (callback != nullptr)
        {
            callback(cookie);
        }
    }
}

void ObserverMessageProcessor::notify_log_update_scopes(const areg::MessageEnvelope& msgReceived)
{
    FuncLogUpdateScopes callback{ nullptr };
    const ITEM_ID cookie{ msgReceived.source() };
    const DateTime now(DateTime::now());

    do
    {
        Lock lock(mLoggerClient.mLock);
        callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogUpdatedScopes : nullptr;
    } while (false);

    ScopeInfo* scopes{ nullptr };
    const uint32_t count{ _save_scopes(msgReceived, cookie, now, scopes) };

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
uint32_t ObserverMessageProcessor::notify_log_message(const areg::MessageEnvelope& msgReceived)
{
    if (areg::is_log_entry_valid(msgReceived) == false)
        return 0u;

    const uint32_t outerLogId{ _deliveredLogId };
    FuncLogMessage callback{ nullptr };
    FuncLogMessageEx callbackEx{ nullptr };

    const areg::LogEntry* msgRemote = reinterpret_cast<const areg::LogEntry*>(msgReceived.buffer());
    ASSERT(msgRemote != nullptr);
    const_cast<areg::LogEntry*>(msgRemote)->logReceived = static_cast<TIME64>(DateTime::now());
    const uint32_t logId{ mLoggerClient.mDbWriter.queue_log(msgReceived) };

    if (LogObserverBase::_theLogObserver == nullptr)
    {
        Lock lock(mLoggerClient.mLock);
        if (mLoggerClient.mCallbacks != nullptr)
        {
            callback   = mLoggerClient.mCallbacks->evtLogMessage;
            callbackEx = callback == nullptr ? mLoggerClient.mCallbacks->evtLogMessageEx : nullptr;
        }
    }

    _deliveredLogId = logId;
    if (LogObserverBase::_theLogObserver != nullptr)
    {
        LogObserverBase::_theLogObserver->on_log_message(msgReceived);
    }
    else if (callback != nullptr)
    {
        // Built here, not above: the record is large and only this callback receives it.
        LogRecord msgLog{ };

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

        areg::mem_copy(msgLog.msgLogText, LENGTH_MESSAGE , msgRemote->logMessage , areg::log_message_size(*msgRemote) + 1);
        areg::mem_copy(msgLog.msgThread,  LENGTH_NAME    , msgRemote->logThread  , msgRemote->logThreadLen  + 1);
        areg::mem_copy(msgLog.msgModule,  LENGTH_NAME    , msgRemote->logModule  , msgRemote->logModuleLen  + 1);

        callback(&msgLog);
    }
    else if (callbackEx != nullptr)
    {
        callbackEx(msgReceived.buffer(), msgReceived.size_used());
    }

    _deliveredLogId = outerLogId;
    return logId;
}
uint32_t ObserverMessageProcessor::delivered_log_id() noexcept
{
    return _deliveredLogId;
}

uint32_t ObserverMessageProcessor::add_local_log(ITEM_ID cookie, areg::LogPriority prio, TIME64 timestamp, const char * message)
{
    areg::LogEntry log;
    _init_local_log_message(log, cookie, timestamp);
    log.logMessagePrio = prio;
    if (message != nullptr)
    {
        // The length is the one before the cut, so a reader can tell how much text is missing.
        log.logMessageLen = static_cast<uint32_t>(areg::string_length<char>(message));
        String::format_string(log.logMessage, static_cast<int32_t>(areg::LOG_MSG_SIZE), "%s", message);
    }

    areg::MessageEnvelope msgLog = areg::create_log_message(log, areg::LogDataType::Local, cookie);
    return notify_log_message(msgLog);
}

bool ObserverMessageProcessor::remove_log(uint32_t logId)
{
    return (logId != 0u) && mLoggerClient.mDbWriter.queue_task([logId](areg::ext::LogSqliteDatabase& database)
        {
            database.remove_log(logId);
        });
}

uint32_t ObserverMessageProcessor::_save_scopes(const areg::MessageEnvelope& msgReceived, ITEM_ID cookie, const DateTime& now, ScopeInfo*& scopes)
{
    uint32_t count{ 0u };
    msgReceived >> count;
    scopes = count != 0u ? new ScopeInfo[count] : nullptr;
    if (scopes == nullptr)
    {
        count = 0u;
    }

    areg::ScopeNames names(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        const uint32_t scopeId   { msgReceived.read32_bits() };
        const uint32_t scopePrio { msgReceived.read32_bits() };
        String         scopeName (msgReceived);
        ScopeInfo &    entry     { scopes[i] };
        entry.lsId   = scopeId;
        entry.lsPrio = scopePrio;
        areg::mem_copy(entry.lsName, LENGTH_SCOPE, scopeName.as_string(), static_cast<uint32_t>(scopeName.length()) + 1u);
        names.add(areg::ScopeEntry(scopeName, scopeId, scopePrio));
    }

    mLoggerClient.mDbWriter.queue_task([names = std::move(names), cookie, now](areg::ext::LogSqliteDatabase& database)
        {
            database.log_scopes_deactivate(cookie, now);
            database.log_scopes_activate(names, cookie, now);
        });

    return count;
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

    const bool toObject{ LogObserverBase::_theLogObserver != nullptr };
    const DateTime now(DateTime::now());
    std::vector<String> texts;

    do
    {
        Lock lock(mLoggerClient.mLock);
        if (toObject == false)
        {
            callback = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstConnected : nullptr;
            listInstances = new LogInstance[static_cast<size_t>(size)];
        }

        for (int i = 0; i < size; ++i)
        {
            const areg::ConnectedInstance& client{ listConnected[static_cast<uint32_t>(i)] };
            auto added = mLoggerClient.mInstances.add_if_unique(client.ciCookie, client, false);
            if (added.second)
            {
                mLoggerClient.mDbWriter.queue_task([client, now](areg::ext::LogSqliteDatabase& database)
                    {
                        database.log_instance_connected(client, now);
                    });

                String text;
                text.format( toObject ? "Log observer have got %u-bit %s (%u) client connection event, ready to receive logs."
                                      : "Log observer have got %u-bit %s (%u) client connection event, starts receiving logs."
                           , static_cast<uint32_t>(client.ciBitness)
                           , client.ciInstance.c_str()
                           , client.ciCookie);
                texts.push_back(text);
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
    } while (false);

    for (const String& text : texts)
    {
        add_local_log(areg::COOKIE_LOGGER, areg::LogPriority::PrioAny, now, text.as_string());
    }

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
            const DateTime now(DateTime::now());
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
                        const ITEM_ID cookie{ client };
                        mLoggerClient.mDbWriter.queue_task([cookie, now](areg::ext::LogSqliteDatabase& database)
                            {
                                database.log_instance_disconnected(cookie, now);
                            });
                    }

                    if (listInstances != nullptr)
                    {
                        listInstances[i] = client;
                        ++count;
                    }
                }
            }
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
