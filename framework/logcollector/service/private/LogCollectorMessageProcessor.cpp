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

LogCollectorMessageProcessor::LogCollectorMessageProcessor(logcollector::LogCollectorServerService & loggerService)
    : mLoggerService    ( loggerService )
    , mListSaveConfig   ( )
    , mPendingSave      ( areg::COOKIE_UNKNOWN )
{
}

void LogCollectorMessageProcessor::queryConnectedInstances(const areg::RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceQueryInstances));

    const ITEM_ID & source{ msgReceived.getSource() };
    const ITEM_ID& target{ msgReceived.getTarget() };
    if ((source >= areg::COOKIE_REMOTE_SERVICE) && (target == areg::COOKIE_LOGGER))
    {
        const areg::MapInstances& instances = mLoggerService.getInstances();
        auto srcPos = instances.find(source);
        if (instances.isValidPosition(srcPos))
        {
            const areg::ConnectedInstance& instance = instances.valueAtPosition(srcPos);
            if (isLogObserver(instance.ciSource))
            {
                notifyConnectedInstances(mLoggerService.getInstances(), source);
            }
        }
    }
}

void LogCollectorMessageProcessor::notifyConnectedInstances(const areg::MapInstances& instances, const ITEM_ID& target /*= areg::TARGET_ALL*/) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.isEmpty())
        return;

    areg::RemoteMessage msgInstances;
    ASSERT((target == areg::TARGET_ALL) || (instances.contains(target) && isLogObserver(instances.getAt(target).ciSource)));

    if (msgInstances.initMessage(areg::getMessageNotifyInstances().rbHeader) != nullptr)
    {
        msgInstances << areg::RemoteConnectionState::Connected;
        uint32_t count{ 0 };
        uint32_t pos = msgInstances.getPosition();
        msgInstances << count; // reserves space, initially set 0
        for (const auto& entry : instances.getData())
        {
            if (LogCollectorMessageProcessor::isLogSource(entry.second.ciSource))
            {
                ++count;
                msgInstances << entry.second;
            }
        }

        if (count != 0)
        {
            msgInstances.setPosition(static_cast<int32_t>(pos), areg::Cursor::SeekOrigin::Begin);
            msgInstances << count;
            msgInstances.moveToEnd();
        }

        if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.getData())
            {
                areg::RemoteMessage msg{ msgInstances.clone(areg::COOKIE_LOGGER, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
        else
        {
            msgInstances.setSource(areg::COOKIE_LOGGER);
            msgInstances.setTarget(target);
            mLoggerService.sendMessage(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::notifyDisconnectedInstances(const areg::ArrayList<ITEM_ID>& listIds, const ITEM_ID& target) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.isEmpty())
        return;

    areg::RemoteMessage msgInstances;
    if (msgInstances.initMessage(areg::getMessageNotifyInstances().rbHeader) != nullptr)
    {
        msgInstances << areg::RemoteConnectionState::Disconnected;
        msgInstances << listIds;
        if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.getData())
            {
                areg::RemoteMessage msg{ msgInstances.clone(areg::COOKIE_LOGGER, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
        else
        {
            msgInstances.setSource(areg::COOKIE_LOGGER);
            msgInstances.setTarget(target);
            mLoggerService.sendMessage(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::registerScopesAtObserver(const areg::RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes));
    msgReceived.moveToBegin();
    uint32_t scopeCount{ 0 };
    msgReceived >> scopeCount;
    msgReceived.moveToBegin();

    areg::String msgStatus;
    constexpr char fmt[]{ "Received scope registration from source %u, sent %u scopes to register ..." };
    msgStatus.format(fmt, static_cast<uint32_t>(msgReceived.getSource()), scopeCount);

    logcollector::LogCollector::printStatus(msgStatus);
    _forwardMessageToObservers(msgReceived);
}

void LogCollectorMessageProcessor::updateLogSourceScopes(const areg::RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogUpdateScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::queryLogSourceScopes(const areg::RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogQueryScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::saveLogSourceConfiguration(const areg::RemoteMessage & msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceSaveLogConfiguration));

    ITEM_ID target{ areg::COOKIE_UNKNOWN };
    msgReceived >> target;
    if ((target == areg::TARGET_ALL) || (target == areg::COOKIE_LOGGER))
    {
        const areg::MapInstances& instances{ mLoggerService.getInstances() };
        for (const auto& entry : instances.getData())
        {
            if (isLogSource(entry.second.ciSource))
            {
                mListSaveConfig.addIfUnique(entry.first);
            }
        }
    }
    else if (target != areg::COOKIE_UNKNOWN)
    {
        mListSaveConfig.addIfUnique(target);
    }

    if ((mPendingSave == areg::COOKIE_UNKNOWN) && (mListSaveConfig.isEmpty() == false))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::logSourceScopesUpadated(const areg::RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
    _forwardMessageToObservers(msgReceived);
}

#ifdef      DEBUG
void LogCollectorMessageProcessor::logSourceConfigurationSaved(const areg::RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogConfigurationSaved));
    processNextSaveConfig();
}
#else   // DEBUG
void LogCollectorMessageProcessor::logSourceConfigurationSaved(const areg::RemoteMessage& /*msgReceived*/)
{
    processNextSaveConfig();
}
#endif  // DEBUG

void LogCollectorMessageProcessor::processNextSaveConfig()
{
    mLoggerService.mSaveTimer.stopTimer();
    mPendingSave = areg::COOKIE_UNKNOWN;
    if (mListSaveConfig.isEmpty() == false)
    {
        mPendingSave = mListSaveConfig[0u];
        mListSaveConfig.removeAt(0u, 1u);
        mLoggerService.sendMessage(areg::messageSaveConfiguration(areg::COOKIE_LOGGER, mPendingSave));
        mLoggerService.mSaveTimer.startTimer(logcollector::LogCollectorServerService::TIMEOUT_SAVE_CONFIG, static_cast<areg::DispatcherThread &>(mLoggerService), 1);
    }
}

void LogCollectorMessageProcessor::clientDisconnected(const ITEM_ID& cookie)
{
    if ((cookie > areg::TARGET_ALL) && (mPendingSave == cookie))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::logMessage(const areg::RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage));
    ASSERT(areg::LogDataType::Remote == reinterpret_cast<const areg::LogEntry *>(msgReceived.getBuffer())->logDataType);
    _forwardMessageToObservers(msgReceived);
}

bool LogCollectorMessageProcessor::isLogSource(areg::MessageSource msgSource)
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

bool LogCollectorMessageProcessor::isLogObserver(areg::MessageSource msgSource)
{
    return ((static_cast<uint32_t>(areg::MessageSource::SourceObserver) & static_cast<uint32_t>(msgSource)) != 0);
}

inline void LogCollectorMessageProcessor::_forwardMessageToLogSources(const areg::RemoteMessage& msgReceived) const
{
    const auto& instances = mLoggerService.getInstances();
    if (instances.isEmpty())
        return;

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ msgReceived.getTarget() != areg::COOKIE_LOGGER ? msgReceived.getTarget() : areg::TARGET_ALL };

    auto srcPos = source != areg::COOKIE_LOGGER ? instances.find(source) : instances.invalidPosition();
    if ((source == areg::COOKIE_LOGGER) || (instances.isValidPosition(srcPos) && isLogObserver(instances.valueAtPosition(srcPos).ciSource)))
    {
        auto dstPos = instances.find(target);
        if (instances.isValidPosition(dstPos) && isLogSource(instances.valueAtPosition(dstPos).ciSource))
        {
            mLoggerService.sendMessage(msgReceived);
        }
        else if (target == areg::TARGET_ALL)
        {
            for (const auto& instance : instances.getData())
            {
                if (isLogSource(instance.second.ciSource))
                {
                    areg::RemoteMessage msg{ msgReceived.clone(source, instance.first) };
                    mLoggerService.sendMessage(msg);
                }
            }
        }
    }
}

inline void LogCollectorMessageProcessor::_forwardMessageToObservers(const areg::RemoteMessage& msgReceived) const
{
    const auto& observers = mLoggerService.getObservers();
    if (observers.isEmpty())
        return;

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ msgReceived.getTarget() != areg::COOKIE_LOGGER ? msgReceived.getTarget() : areg::TARGET_ALL };
    const areg::MapInstances& instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos) && isLogSource(instances.valueAtPosition(srcPos).ciSource))
    {
        if (instances.isValidPosition(dstPos) && isLogObserver(instances.valueAtPosition(dstPos).ciSource))
        {
            mLoggerService.sendMessage(msgReceived);
        }
        else if (target == areg::TARGET_ALL)
        {
            for (const auto& observer : observers.getData())
            {
                ASSERT(isLogObserver(observer.second.ciSource));
                areg::RemoteMessage msg{ msgReceived.clone(source, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
    }
}
