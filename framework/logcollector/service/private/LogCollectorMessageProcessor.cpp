/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        logcollector/service/private/LogCollectorMessageProcessor.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector message processor
 ************************************************************************/
#include "logcollector/service/private/LogCollectorMessageProcessor.hpp"

#include "logcollector/app/LogCollector.hpp"
#include "logcollector/service/LogCollectorServerService.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"

LogCollectorMessageProcessor::LogCollectorMessageProcessor(LogCollectorServerService & loggerService)
    : mLoggerService    ( loggerService )
    , mListSaveConfig   ( )
    , mPendingSave      ( areg::COOKIE_UNKNOWN )
{
}

void LogCollectorMessageProcessor::query_connected_instances(const areg::EventEnvelope & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances));

    const ITEM_ID source{ static_cast<ITEM_ID>(msgReceived.source()) };
    const ITEM_ID target{ static_cast<ITEM_ID>(msgReceived.target()) };
    if ((source >= areg::COOKIE_REMOTE_SERVICE) && (target == areg::COOKIE_LOGGER))
    {
        const areg::MapInstances& observers = mLoggerService.observers();
        auto srcPos = observers.find(source);
        if (observers.is_valid_position(srcPos))
        {
            const areg::ConnectedInstance& instance = observers.value_at(srcPos);
            if (is_log_observer(instance.ciSource))
            {
                notify_connected_instances(mLoggerService.instances(), source);
            }
        }
    }
}

void LogCollectorMessageProcessor::notify_connected_instances(const areg::MapInstances& instances, const ITEM_ID& target /*= areg::TARGET_ALL*/) const
{
    const auto& observers{ mLoggerService.observers() };
    if (observers.is_empty())
        return;

    areg::EventEnvelope msgInstances;
    ASSERT((target == areg::TARGET_ALL) || (instances.contains(target) && is_log_observer(instances.value_at(target).ciSource)));

    static constexpr areg::EventHeader HDR_NOTIFY{ areg::message_notify_instances() };
    if (msgInstances.init_envelope(HDR_NOTIFY) != nullptr)
    {
        msgInstances << areg::RemoteConnectionState::Connected;
        uint32_t count{ 0 };
        uint32_t pos = msgInstances.position();
        msgInstances << count; // reserves space, initially set 0
        for (const auto& entry : instances.data())
        {
            if (LogCollectorMessageProcessor::is_log_source(entry.second.ciSource))
            {
                ++count;
                msgInstances << entry.second;
            }
        }

        if (count != 0)
        {
            msgInstances.set_position(static_cast<int32_t>(pos), areg::Cursor::SeekOrigin::Begin);
            msgInstances << count;
            msgInstances.move_to_end();
        }

        if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                areg::EventEnvelope msg{ msgInstances.clone() };
                msg.set_source(static_cast<uint32_t>(areg::COOKIE_LOGGER));
                msg.set_target(static_cast<uint32_t>(observer.first));
                mLoggerService.send_message(msg);
            }
        }
        else
        {
            msgInstances.set_source(static_cast<uint32_t>(areg::COOKIE_LOGGER));
            msgInstances.set_target(static_cast<uint32_t>(target));
            mLoggerService.send_message(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::notify_disconnected_instances(const areg::ArrayList<ITEM_ID>& listIds, const ITEM_ID& target) const
{
    const auto& observers{ mLoggerService.observers() };
    if (observers.is_empty())
        return;

    areg::EventEnvelope msgInstances;
    static constexpr areg::EventHeader HDR_NOTIFY{ areg::message_notify_instances() };
    if (msgInstances.init_envelope(HDR_NOTIFY) != nullptr)
    {
        msgInstances << areg::RemoteConnectionState::Disconnected;
        msgInstances << listIds;
        if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                areg::EventEnvelope msg{ msgInstances.clone() };
                msg.set_source(static_cast<uint32_t>(areg::COOKIE_LOGGER));
                msg.set_target(static_cast<uint32_t>(observer.first));
                mLoggerService.send_message(msg);
            }
        }
        else
        {
            msgInstances.set_source(static_cast<uint32_t>(areg::COOKIE_LOGGER));
            msgInstances.set_target(static_cast<uint32_t>(target));
            mLoggerService.send_message(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::register_scopes_at_observer(const areg::EventEnvelope & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes));
    msgReceived.move_to_begin();
    uint32_t scopeCount{ 0 };
    msgReceived >> scopeCount;
    msgReceived.move_to_begin();

    areg::String msgStatus;
    constexpr char fmt[]{ "Received scope registration from source %u, sent %u scopes to register ..." };
    msgStatus.format(fmt, static_cast<uint32_t>(msgReceived.source()), scopeCount);

    LogCollector::print_status(msgStatus);
    _forward_message_to_observers(msgReceived);
}

void LogCollectorMessageProcessor::update_log_source_scopes(const areg::EventEnvelope & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
    _forward_message_to_log_sources(msgReceived);
}

void LogCollectorMessageProcessor::query_log_source_scopes(const areg::EventEnvelope & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogQueryScopes));
    _forward_message_to_log_sources(msgReceived);
}

void LogCollectorMessageProcessor::save_log_source_configuration(const areg::EventEnvelope & msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceSaveLogConfiguration));

    ITEM_ID target{ areg::COOKIE_UNKNOWN };
    msgReceived >> target;
    if ((target == areg::TARGET_ALL) || (target == areg::COOKIE_LOGGER))
    {
        const areg::MapInstances& instances{ mLoggerService.instances() };
        for (const auto& entry : instances.data())
        {
            if (is_log_source(entry.second.ciSource))
            {
                mListSaveConfig.add_if_unique(entry.first);
            }
        }
    }
    else if (target != areg::COOKIE_UNKNOWN)
    {
        mListSaveConfig.add_if_unique(target);
    }

    if ((mPendingSave == areg::COOKIE_UNKNOWN) && (mListSaveConfig.is_empty() == false))
    {
        process_next_save_config();
    }
}

void LogCollectorMessageProcessor::log_source_scopes_updated(const areg::EventEnvelope& msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
    _forward_message_to_observers(msgReceived);
}

#ifdef      DEBUG
void LogCollectorMessageProcessor::log_source_configuration_saved(const areg::EventEnvelope& msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
    process_next_save_config();
}
#else   // DEBUG
void LogCollectorMessageProcessor::log_source_configuration_saved(const areg::EventEnvelope& /*msgReceived*/)
{
    process_next_save_config();
}
#endif  // DEBUG

void LogCollectorMessageProcessor::process_next_save_config()
{
    mLoggerService.mSaveTimer.stop_timer();
    mPendingSave = areg::COOKIE_UNKNOWN;
    if (mListSaveConfig.is_empty() == false)
    {
        mPendingSave = mListSaveConfig[0u];
        mListSaveConfig.remove_at(0u, 1u);
        mLoggerService.send_message(areg::message_save_configuration(areg::COOKIE_LOGGER, mPendingSave));
        mLoggerService.mSaveTimer.start_timer(LogCollectorServerService::TIMEOUT_SAVE_CONFIG, static_cast<areg::DispatcherThread &>(mLoggerService), 1);
    }
}

void LogCollectorMessageProcessor::client_disconnected(const ITEM_ID& cookie)
{
    if ((cookie > areg::TARGET_ALL) && (mPendingSave == cookie))
    {
        process_next_save_config();
    }
}

void LogCollectorMessageProcessor::log_message(const areg::EventEnvelope & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage));
    ASSERT(areg::LogDataType::Remote == reinterpret_cast<const areg::LogEntry *>(msgReceived.buffer())->logDataType);
    _forward_message_to_observers(msgReceived);
}

bool LogCollectorMessageProcessor::is_log_source(areg::MessageSource msgSource)
{
    switch (msgSource)
    {
    case areg::MessageSource::SourceClient:    // fall through
    case areg::MessageSource::SourceSimulation:// fall through
    case areg::MessageSource::SourceTest:
        return true;

    case areg::MessageSource::SourceUndefined: // fall through
    case areg::MessageSource::SourceService:   // fall through
    case areg::MessageSource::SourceObserver:  // fall through
    default:
        return false;
    }
}

bool LogCollectorMessageProcessor::is_log_observer(areg::MessageSource msgSource)
{
    return ((static_cast<uint32_t>(areg::MessageSource::SourceObserver) & static_cast<uint32_t>(msgSource)) != 0);
}

inline void LogCollectorMessageProcessor::_forward_message_to_log_sources(const areg::EventEnvelope& msgReceived) const
{
    const auto& instances = mLoggerService.instances();
    if (instances.is_empty())
        return;

    ITEM_ID source{ static_cast<ITEM_ID>(msgReceived.source()) };
    ITEM_ID target{ static_cast<ITEM_ID>(msgReceived.target()) != areg::COOKIE_LOGGER ? static_cast<ITEM_ID>(msgReceived.target()) : areg::TARGET_ALL };

    auto srcPos = source != areg::COOKIE_LOGGER ? instances.find(source) : instances.invalid_position();
    if ((source == areg::COOKIE_LOGGER) || (instances.is_valid_position(srcPos) && is_log_observer(instances.value_at(srcPos).ciSource)))
    {
        auto dstPos = instances.find(target);
        if (instances.is_valid_position(dstPos) && is_log_source(instances.value_at(dstPos).ciSource))
        {
            mLoggerService.send_message(msgReceived);
        }
        else if (target == areg::TARGET_ALL)
        {
            for (const auto& instance : instances.data())
            {
                if (is_log_source(instance.second.ciSource))
                {
                    areg::EventEnvelope msg{ msgReceived.clone() };
                    msg.set_source(static_cast<uint32_t>(source));
                    msg.set_target(static_cast<uint32_t>(instance.first));
                    mLoggerService.send_message(msg);
                }
            }
        }
    }
}

inline void LogCollectorMessageProcessor::_forward_message_to_observers(const areg::EventEnvelope& msgReceived) const
{
    const auto& observers = mLoggerService.observers();
    if (observers.is_empty())
        return;

    ITEM_ID source{ static_cast<ITEM_ID>(msgReceived.source()) };
    ITEM_ID target{ static_cast<ITEM_ID>(msgReceived.target()) != areg::COOKIE_LOGGER ? static_cast<ITEM_ID>(msgReceived.target()) : areg::TARGET_ALL };
    const areg::MapInstances& instances = mLoggerService.instances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.is_valid_position(srcPos) && is_log_source(instances.value_at(srcPos).ciSource))
    {
        if (instances.is_valid_position(dstPos) && is_log_observer(instances.value_at(dstPos).ciSource))
        {
            mLoggerService.send_message(msgReceived);
        }
        else if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                ASSERT(is_log_observer(observer.second.ciSource));
                areg::EventEnvelope msg{ msgReceived.clone() };
                msg.set_source(static_cast<uint32_t>(source));
                msg.set_target(static_cast<uint32_t>(observer.first));
                mLoggerService.send_message(msg);
            }
        }
    }
}
