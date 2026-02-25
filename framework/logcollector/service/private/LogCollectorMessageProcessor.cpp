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
    , mPendingSave      ( NEService::COOKIE_UNKNOWN )
{
}

void LogCollectorMessageProcessor::queryConnectedInstances(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::SystemServiceQueryInstances));

    const ITEM_ID & source{ msgReceived.source() };
    const ITEM_ID& target{ msgReceived.target() };
    if ((source >= NEService::COOKIE_REMOTE_SERVICE) && (target == NEService::COOKIE_LOGGER))
    {
        const NEService::MapInstances& instances = mLoggerService.getInstances();
        auto srcPos = instances.find(source);
        if (instances.is_valid_position(srcPos))
        {
            const NEService::ConnectedInstance& instance = instances.value_at_position(srcPos);
            if (isLogObserver(instance.ciSource))
            {
                notifyConnectedInstances(mLoggerService.getInstances(), source);
            }
        }
    }
}

void LogCollectorMessageProcessor::notifyConnectedInstances(const NEService::MapInstances& instances, const ITEM_ID& target /*= NEService::TARGET_ALL*/) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.is_empty())
        return;

    RemoteMessage msgInstances;
    ASSERT((target == NEService::TARGET_ALL) || (instances.contains(target) && isLogObserver(instances.at(target).ciSource)));

    if (msgInstances.init_message(NERemoteService::message_notify_instances().rbHeader) != nullptr)
    {
        msgInstances << NERemoteService::RemoteConnectionState::Connected;
        uint32_t count{ 0 };
        uint32_t pos = msgInstances.position();
        msgInstances << count; // reserves space, initially set 0
        for (const auto& entry : instances.data())
        {
            if (LogCollectorMessageProcessor::isLogSource(entry.second.ciSource))
            {
                ++count;
                msgInstances << entry.second;
            }
        }

        if (count != 0)
        {
            msgInstances.set_position(static_cast<int32_t>(pos), Cursor::SeekOrigin::Begin);
            msgInstances << count;
            msgInstances.move_to_end();
        }

        if (target == NEService::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                RemoteMessage msg{ msgInstances.clone(NEService::COOKIE_LOGGER, observer.first) };
                mLoggerService.send_message(msg);
            }
        }
        else
        {
            msgInstances.set_source(NEService::COOKIE_LOGGER);
            msgInstances.set_target(target);
            mLoggerService.send_message(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::notifyDisconnectedInstances(const ArrayList<ITEM_ID>& listIds, const ITEM_ID& target) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.is_empty())
        return;

    RemoteMessage msgInstances;
    if (msgInstances.init_message(NERemoteService::message_notify_instances().rbHeader) != nullptr)
    {
        msgInstances << NERemoteService::RemoteConnectionState::Disconnected;
        msgInstances << listIds;
        if (target == NEService::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                RemoteMessage msg{ msgInstances.clone(NEService::COOKIE_LOGGER, observer.first) };
                mLoggerService.send_message(msg);
            }
        }
        else
        {
            msgInstances.set_source(NEService::COOKIE_LOGGER);
            msgInstances.set_target(target);
            mLoggerService.send_message(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::registerScopesAtObserver(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogRegisterScopes));
    msgReceived.move_to_begin();
    uint32_t scopeCount{ 0 };
    msgReceived >> scopeCount;
    msgReceived.move_to_begin();

    String msgStatus;
    constexpr char fmt[]{ "Received scope registration from source %u, sent %u scopes to register ..." };
    msgStatus.format(fmt, static_cast<uint32_t>(msgReceived.source()), scopeCount);

    LogCollector::printStatus(msgStatus);
    _forwardMessageToObservers(msgReceived);
}

void LogCollectorMessageProcessor::updateLogSourceScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogUpdateScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::queryLogSourceScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogQueryScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::saveLogSourceConfiguration(const RemoteMessage & msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceSaveLogConfiguration));

    ITEM_ID target{ NEService::COOKIE_UNKNOWN };
    msgReceived >> target;
    if ((target == NEService::TARGET_ALL) || (target == NEService::COOKIE_LOGGER))
    {
        const NEService::MapInstances& instances{ mLoggerService.getInstances() };
        for (const auto& entry : instances.data())
        {
            if (isLogSource(entry.second.ciSource))
            {
                mListSaveConfig.add_if_unique(entry.first);
            }
        }
    }
    else if (target != NEService::COOKIE_UNKNOWN)
    {
        mListSaveConfig.add_if_unique(target);
    }

    if ((mPendingSave == NEService::COOKIE_UNKNOWN) && (mListSaveConfig.is_empty() == false))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::logSourceScopesUpadated(const RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogScopesUpdated));
    _forwardMessageToObservers(msgReceived);
}

#ifdef      DEBUG
void LogCollectorMessageProcessor::logSourceConfigurationSaved(const RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogConfigurationSaved));
    processNextSaveConfig();
}
#else   // DEBUG
void LogCollectorMessageProcessor::logSourceConfigurationSaved(const RemoteMessage& /*msgReceived*/)
{
    processNextSaveConfig();
}
#endif  // DEBUG

void LogCollectorMessageProcessor::processNextSaveConfig()
{
    mLoggerService.mSaveTimer.stop_timer();
    mPendingSave = NEService::COOKIE_UNKNOWN;
    if (mListSaveConfig.is_empty() == false)
    {
        mPendingSave = mListSaveConfig[0u];
        mListSaveConfig.remove_at(0u, 1u);
        mLoggerService.send_message(NELogging::message_save_configuration(NEService::COOKIE_LOGGER, mPendingSave));
        mLoggerService.mSaveTimer.start_timer(LogCollectorServerService::TIMEOUT_SAVE_CONFIG, static_cast<DispatcherThread &>(mLoggerService), 1);
    }
}

void LogCollectorMessageProcessor::clientDisconnected(const ITEM_ID& cookie)
{
    if ((cookie > NEService::TARGET_ALL) && (mPendingSave == cookie))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::log_message(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.message_id() == static_cast<uint32_t>(NEService::FuncIdRange::ServiceLogMessage));
    ASSERT(NELogging::LogDataType::Remote == reinterpret_cast<const NELogging::LogEntry *>(msgReceived.buffer())->logDataType);
    _forwardMessageToObservers(msgReceived);
}

bool LogCollectorMessageProcessor::isLogSource(NEService::MessageSource msgSource)
{
    switch (msgSource)
    {
    case NEService::MessageSource::SourceClient:    // fall through
    case NEService::MessageSource::SourceSimulation:// fall through
    case NEService::MessageSource::SourceTest:
        return true;

    case NEService::MessageSource::SourceUndefined: // fall through
    case NEService::MessageSource::SourceService:   // fall through
    case NEService::MessageSource::SourceObserver:  // fall through
    default:
        return false;
    }
}

bool LogCollectorMessageProcessor::isLogObserver(NEService::MessageSource msgSource)
{
    return ((static_cast<uint32_t>(NEService::MessageSource::SourceObserver) & static_cast<uint32_t>(msgSource)) != 0);
}

inline void LogCollectorMessageProcessor::_forwardMessageToLogSources(const RemoteMessage& msgReceived) const
{
    const auto& instances = mLoggerService.getInstances();
    if (instances.is_empty())
        return;

    ITEM_ID source{ msgReceived.source() };
    ITEM_ID target{ msgReceived.target() != NEService::COOKIE_LOGGER ? msgReceived.target() : NEService::TARGET_ALL };

    auto srcPos = source != NEService::COOKIE_LOGGER ? instances.find(source) : instances.invalid_position();
    if ((source == NEService::COOKIE_LOGGER) || (instances.is_valid_position(srcPos) && isLogObserver(instances.value_at_position(srcPos).ciSource)))
    {
        auto dstPos = instances.find(target);
        if (instances.is_valid_position(dstPos) && isLogSource(instances.value_at_position(dstPos).ciSource))
        {
            mLoggerService.send_message(msgReceived);
        }
        else if (target == NEService::TARGET_ALL)
        {
            for (const auto& instance : instances.data())
            {
                if (isLogSource(instance.second.ciSource))
                {
                    RemoteMessage msg{ msgReceived.clone(source, instance.first) };
                    mLoggerService.send_message(msg);
                }
            }
        }
    }
}

inline void LogCollectorMessageProcessor::_forwardMessageToObservers(const RemoteMessage& msgReceived) const
{
    const auto& observers = mLoggerService.getObservers();
    if (observers.is_empty())
        return;

    ITEM_ID source{ msgReceived.source() };
    ITEM_ID target{ msgReceived.target() != NEService::COOKIE_LOGGER ? msgReceived.target() : NEService::TARGET_ALL };
    const NEService::MapInstances& instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.is_valid_position(srcPos) && isLogSource(instances.value_at_position(srcPos).ciSource))
    {
        if (instances.is_valid_position(dstPos) && isLogObserver(instances.value_at_position(dstPos).ciSource))
        {
            mLoggerService.send_message(msgReceived);
        }
        else if (target == NEService::TARGET_ALL)
        {
            for (const auto& observer : observers.data())
            {
                ASSERT(isLogObserver(observer.second.ciSource));
                RemoteMessage msg{ msgReceived.clone(source, observer.first) };
                mLoggerService.send_message(msg);
            }
        }
    }
}
