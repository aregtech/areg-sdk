/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logcollector/service/private/LogCollectorMessageProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector message processor
 ************************************************************************/
#include "logcollector/service/private/LogCollectorMessageProcessor.hpp"

#include "logcollector/app/LogCollector.hpp"
#include "logcollector/service/LogCollectorServerService.hpp"
#include "areg/ipc/NERemoteService.hpp"

LogCollectorMessageProcessor::LogCollectorMessageProcessor(LogCollectorServerService & loggerService)
    : mLoggerService    ( loggerService )
    , mListSaveConfig   ( )
    , mPendingSave      ( NEService::COOKIE_UNKNOWN )
{
}

void LogCollectorMessageProcessor::queryConnectedInstances(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::SystemServiceQueryInstances));

    const ITEM_ID & source{ msgReceived.getSource() };
    const ITEM_ID& target{ msgReceived.getTarget() };
    if ((source >= NEService::COOKIE_REMOTE_SERVICE) && (target == NEService::COOKIE_LOGGER))
    {
        const NEService::MapInstances& instances = mLoggerService.getInstances();
        auto srcPos = instances.find(source);
        if (instances.isValidPosition(srcPos))
        {
            const NEService::sServiceConnectedInstance& instance = instances.valueAtPosition(srcPos);
            if (isLogObserver(instance.ciSource))
            {
                notifyConnectedInstances(mLoggerService.getInstances(), source);
            }
        }
    }
}

void LogCollectorMessageProcessor::notifyConnectedInstances(const NEService::MapInstances& instances, const ITEM_ID& target /*= NEService::COOKIE_ANY*/) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.isEmpty())
        return;

    RemoteMessage msgInstances;
    ASSERT((target == NEService::COOKIE_ANY) || (instances.contains(target) && isLogObserver(instances.getAt(target).ciSource)));

    if (msgInstances.initMessage(NERemoteService::getMessageNotifyInstances().rbHeader) != nullptr)
    {
        msgInstances << NERemoteService::eRemoteConnection::RemoteConnected;
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
            msgInstances.setPosition(static_cast<int>(pos), IECursorPosition::eCursorPosition::PositionBegin);
            msgInstances << count;
            msgInstances.moveToEnd();
        }

        if (target == NEService::COOKIE_ANY)
        {
            for (const auto& observer : observers.getData())
            {
                RemoteMessage msg{ msgInstances.clone(NEService::COOKIE_LOGGER, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
        else
        {
            msgInstances.setSource(NEService::COOKIE_LOGGER);
            msgInstances.setTarget(target);
            mLoggerService.sendMessage(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::notifyDisconnectedInstances(const TEArrayList<ITEM_ID>& listIds, const ITEM_ID& target) const
{
    const auto& observers{ mLoggerService.getObservers() };
    if (observers.isEmpty())
        return;

    RemoteMessage msgInstances;
    if (msgInstances.initMessage(NERemoteService::getMessageNotifyInstances().rbHeader) != nullptr)
    {
        msgInstances << NERemoteService::eRemoteConnection::RemoteDisconnected;
        msgInstances << listIds;
        if (target == NEService::COOKIE_ANY)
        {
            for (const auto& observer : observers.getData())
            {
                RemoteMessage msg{ msgInstances.clone(NEService::COOKIE_LOGGER, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
        else
        {
            msgInstances.setSource(NEService::COOKIE_LOGGER);
            msgInstances.setTarget(target);
            mLoggerService.sendMessage(msgInstances);
        }
    }
}

void LogCollectorMessageProcessor::registerScopesAtObserver(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));
    msgReceived.moveToBegin();
    uint32_t scopeCount{ 0 };
    msgReceived >> scopeCount;
    msgReceived.moveToBegin();

    String msgStatus;
    constexpr char fmt[]{ "Received scope registration from source %u, sent %u scopes to register ..." };
    msgStatus.format(fmt, static_cast<uint32_t>(msgReceived.getSource()), scopeCount);

    LogCollector::printStatus(msgStatus);
    _forwardMessageToObservers(msgReceived);
}

void LogCollectorMessageProcessor::updateLogSourceScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogUpdateScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::queryLogSourceScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogQueryScopes));
    _forwardMessageToLogSources(msgReceived);
}

void LogCollectorMessageProcessor::saveLogSourceConfiguration(const RemoteMessage & msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceSaveLogConfiguration));

    ITEM_ID target{ NEService::COOKIE_UNKNOWN };
    msgReceived >> target;
    if ((target == NEService::COOKIE_ANY) || (target == NEService::COOKIE_LOGGER))
    {
        const NEService::MapInstances& instances{ mLoggerService.getInstances() };
        for (const auto& entry : instances.getData())
        {
            if (isLogSource(entry.second.ciSource))
            {
                mListSaveConfig.addIfUnique(entry.first);
            }
        }
    }
    else if (target != NEService::COOKIE_UNKNOWN)
    {
        mListSaveConfig.addIfUnique(target);
    }

    if ((mPendingSave == NEService::COOKIE_UNKNOWN) && (mListSaveConfig.isEmpty() == false))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::logSourceScopesUpadated(const RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogScopesUpdated));
    _forwardMessageToObservers(msgReceived);
}

void LogCollectorMessageProcessor::logSourceConfigurationSaved(const RemoteMessage& msgReceived)
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogConfigurationSaved));
    processNextSaveConfig();
}

void LogCollectorMessageProcessor::processNextSaveConfig(void)
{
    mLoggerService.mSaveTimer.stopTimer();
    mPendingSave = NEService::COOKIE_UNKNOWN;
    if (mListSaveConfig.isEmpty() == false)
    {
        mPendingSave = mListSaveConfig[0u];
        mListSaveConfig.removeAt(0u, 1u);
        mLoggerService.sendMessage(NELogging::messageSaveConfiguration(NEService::COOKIE_LOGGER, mPendingSave));
        mLoggerService.mSaveTimer.startTimer(LogCollectorServerService::TIMEOUT_SAVE_CONFIG, static_cast<DispatcherThread &>(mLoggerService), 1);
    }
}

void LogCollectorMessageProcessor::clientDisconnected(const ITEM_ID& cookie)
{
    if ((cookie > NEService::COOKIE_ANY) && (mPendingSave == cookie))
    {
        processNextSaveConfig();
    }
}

void LogCollectorMessageProcessor::logMessage(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogMessage));
    ASSERT(NELogging::eLogDataType::LogDataRemote == reinterpret_cast<const NELogging::sLogMessage *>(msgReceived.getBuffer())->logDataType);
    _forwardMessageToObservers(msgReceived);
}

bool LogCollectorMessageProcessor::isLogSource(NEService::eMessageSource msgSource)
{
    switch (msgSource)
    {
    case NEService::eMessageSource::MessageSourceClient:    // fall through
    case NEService::eMessageSource::MessageSourceSimulation:// fall through
    case NEService::eMessageSource::MessageSourceTest:
        return true;

    case NEService::eMessageSource::MessageSourceUndefined: // fall through
    case NEService::eMessageSource::MessageSourceService:   // fall through
    case NEService::eMessageSource::MessageSourceObserver:  // fall through
    default:
        return false;
    }
}

bool LogCollectorMessageProcessor::isLogObserver(NEService::eMessageSource msgSource)
{
    return (NEService::eMessageSource::MessageSourceObserver == msgSource);
}

inline void LogCollectorMessageProcessor::_forwardMessageToLogSources(const RemoteMessage& msgReceived) const
{
    const auto& instances = mLoggerService.getInstances();
    if (instances.isEmpty())
        return;

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ msgReceived.getTarget() != NEService::COOKIE_LOGGER ? msgReceived.getTarget() : NEService::COOKIE_ANY };

    auto srcPos = source != NEService::COOKIE_LOGGER ? instances.find(source) : instances.invalidPosition();
    if ((source == NEService::COOKIE_LOGGER) || (instances.isValidPosition(srcPos) && isLogObserver(instances.valueAtPosition(srcPos).ciSource)))
    {
        auto dstPos = instances.find(target);
        if (instances.isValidPosition(dstPos) && isLogSource(instances.valueAtPosition(dstPos).ciSource))
        {
            mLoggerService.sendMessage(msgReceived);
        }
        else if (target == NEService::COOKIE_ANY)
        {
            for (const auto& instance : instances.getData())
            {
                if (isLogSource(instance.second.ciSource))
                {
                    RemoteMessage msg{ msgReceived.clone(source, instance.first) };
                    mLoggerService.sendMessage(msg);
                }
            }
        }
    }
}

inline void LogCollectorMessageProcessor::_forwardMessageToObservers(const RemoteMessage& msgReceived) const
{
    const auto& observers = mLoggerService.getObservers();
    if (observers.isEmpty())
        return;

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ msgReceived.getTarget() != NEService::COOKIE_LOGGER ? msgReceived.getTarget() : NEService::COOKIE_ANY };
    const NEService::MapInstances& instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos) && isLogSource(instances.valueAtPosition(srcPos).ciSource))
    {
        if (instances.isValidPosition(dstPos) && isLogObserver(instances.valueAtPosition(dstPos).ciSource))
        {
            mLoggerService.sendMessage(msgReceived);
        }
        else if (target == NEService::COOKIE_ANY)
        {
            for (const auto& observer : observers.getData())
            {
                ASSERT(isLogObserver(observer.second.ciSource));
                RemoteMessage msg{ msgReceived.clone(source, observer.first) };
                mLoggerService.sendMessage(msg);
            }
        }
    }
}
