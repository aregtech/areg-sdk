/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/service/private/LoggerMessageProcessor.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector message processor
 ************************************************************************/

#include "logger/service/private/LoggerMessageProcessor.hpp"

#include "logger/service/LoggerServerService.hpp"
#include "areg/ipc/NEConnection.hpp"

LoggerMessageProcessor::LoggerMessageProcessor(LoggerServerService & loggerService)
    : mLoggerService    ( loggerService )
{
}

void LoggerMessageProcessor::notifyTarget(const ITEM_ID & target, const RemoteMessage & msgNotify) const
{
    RemoteMessage msg{ msgNotify.clone(mLoggerService.getId(), target) };
    mLoggerService.sendMessage(msg);
}

void LoggerMessageProcessor::queryInstances(const RemoteMessage & msgReceived) const
{
    const ITEM_ID & source = msgReceived.getSource();
    const ServiceCommunicatonBase::sConnectedInstance & value = mLoggerService.getInstances().getAt(source);
    if (value.ciSource == NEService::eMessageSource::MessageSourceObserver)
    {
        notifyInstances();
    }
}

void LoggerMessageProcessor::notifyInstances(void) const
{
    RemoteMessage msgInstances;
    if (msgInstances.initMessage(NEConnection::getMessageQueryInstances().rbHeader) != nullptr)
    {
        const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();
        ServiceCommunicatonBase::MapInstances clients;
        for (auto pos = instances.firstPosition(); instances.isValidPosition(pos); pos = instances.nextPosition(pos))
        {
            const ITEM_ID & key{ instances.keyAtPosition(pos) };
            const ServiceCommunicatonBase::sConnectedInstance & value{ instances.valueAtPosition(pos) };
            if (value.ciSource != NEService::eMessageSource::MessageSourceObserver)
            {
                clients.setAt(key, value);
            }
        }

        const ServiceCommunicatonBase::MapInstances & observers = mLoggerService.getObservers();
        if (observers.isEmpty() == false)
        {
            msgInstances << clients;
            for (auto pos = observers.firstPosition(); observers.isValidPosition(pos); pos = observers.nextPosition(pos))
            {
                notifyTarget(observers.keyAtPosition(pos), msgInstances);
            }
        }
    }
}

void LoggerMessageProcessor::notifyTargetInstances(const ITEM_ID & target) const
{
    RemoteMessage msgInstances;
    if (msgInstances.initMessage(NEConnection::getMessageQueryInstances().rbHeader) != nullptr)
    {
        const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();
        ServiceCommunicatonBase::MapInstances clients;
        for (auto pos = instances.firstPosition(); instances.isValidPosition(pos); pos = instances.nextPosition(pos))
        {
            const ITEM_ID & key{ instances.keyAtPosition(pos) };
            const ServiceCommunicatonBase::sConnectedInstance & value{ instances.valueAtPosition(pos) };
            if (value.ciSource != NEService::eMessageSource::MessageSourceObserver)
            {
                clients.setAt(key, value);
            }
        }

        msgInstances << clients;
        msgInstances.setSource(mLoggerService.getId());
        msgInstances.setTarget(target);
        mLoggerService.sendMessage(msgInstances);
    }
}

void LoggerMessageProcessor::registerScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogRegisterScopes));

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ NEService::COOKIE_ANY };
    msgReceived >> target;

    const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos))
    {
        if (instances.isValidPosition(dstPos))
        {
            RemoteMessage msg{ msgReceived.clone(source, target) };
            mLoggerService.sendMessage(msg);
        }
        else if (instances.valueAtPosition(srcPos).ciSource != NEService::eMessageSource::MessageSourceObserver)
        {
            const ServiceCommunicatonBase::MapInstances & observers = mLoggerService.getObservers();
            for (auto pos = observers.firstPosition(); observers.isInvalidPosition(pos); pos = observers.nextPosition(pos))
            {
                const ITEM_ID & key{ observers.keyAtPosition(pos) };
                RemoteMessage msg{ msgReceived.clone(source, key) };
                mLoggerService.sendMessage(msg);
            }
        }
    }
}

void LoggerMessageProcessor::updateScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogUpdateScopes));

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ NEService::COOKIE_ANY };
    msgReceived >> target;

    const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos))
    {
        if (instances.isValidPosition(dstPos))
        {
            RemoteMessage msg{ msgReceived.clone(source, target) };
            mLoggerService.sendMessage(msg);
        }
        else if (instances.valueAtPosition(srcPos).ciSource != NEService::eMessageSource::MessageSourceObserver)
        {
            const ServiceCommunicatonBase::MapInstances & observers = mLoggerService.getObservers();
            for (auto pos = observers.firstPosition(); observers.isInvalidPosition(pos); pos = observers.nextPosition(pos))
            {
                const ITEM_ID & key{ observers.keyAtPosition(pos) };
                RemoteMessage msg{ msgReceived.clone(source, key) };
                mLoggerService.sendMessage(msg);
            }
        }
    }
}

void LoggerMessageProcessor::queryScopes(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogQueryScopes));

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ NEService::COOKIE_ANY };
    msgReceived >> target;

    const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos))
    {
        if (instances.isValidPosition(dstPos))
        {
            RemoteMessage msg{ msgReceived.clone(source, target) };
            mLoggerService.sendMessage(msg);
        }
        else if (instances.valueAtPosition(srcPos).ciSource == NEService::eMessageSource::MessageSourceObserver)
        {
            for (auto pos = instances.firstPosition(); instances.isInvalidPosition(pos); pos = instances.nextPosition(pos))
            {
                const ServiceCommunicatonBase::sConnectedInstance & value{ instances.valueAtPosition(pos) };
                if (value.ciSource != NEService::eMessageSource::MessageSourceObserver)
                {
                    const ITEM_ID & key{ instances.keyAtPosition(pos) };
                    RemoteMessage msg{ msgReceived.clone(source, key) };
                    mLoggerService.sendMessage(msg);
                }
            }
        }
    }
}

void LoggerMessageProcessor::saveLogConfiguration(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceSaveLogConfiguration));

    ITEM_ID source{ msgReceived.getSource() };
    ITEM_ID target{ NEService::COOKIE_ANY };
    msgReceived >> target;

    const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();

    auto srcPos = instances.find(source);
    auto dstPos = instances.find(target);
    if (instances.isValidPosition(srcPos))
    {
        if (instances.isValidPosition(dstPos))
        {
            RemoteMessage msg{ msgReceived.clone(source, target) };
            mLoggerService.sendMessage(msg);
        }
        else if (instances.valueAtPosition(srcPos).ciSource == NEService::eMessageSource::MessageSourceObserver)
        {
            for (auto pos = instances.firstPosition(); instances.isInvalidPosition(pos); pos = instances.nextPosition(pos))
            {
                const ServiceCommunicatonBase::sConnectedInstance & value{ instances.valueAtPosition(pos) };
                if (value.ciSource != NEService::eMessageSource::MessageSourceObserver)
                {
                    const ITEM_ID & key{ instances.keyAtPosition(pos) };
                    RemoteMessage msg{ msgReceived.clone(source, key) };
                    mLoggerService.sendMessage(msg);
                }
            }
        }
    }
}

void LoggerMessageProcessor::logMessage(const RemoteMessage & msgReceived) const
{
    ASSERT(msgReceived.getMessageId() == static_cast<uint32_t>(NEService::eFuncIdRange::ServiceLogMessage));

    ITEM_ID source{ msgReceived.getSource() };
    const ServiceCommunicatonBase::MapInstances & instances = mLoggerService.getInstances();
    auto srcPos = instances.find(source);
    if (instances.isValidPosition(srcPos) && (instances.valueAtPosition(srcPos).ciSource != NEService::eMessageSource::MessageSourceObserver))
    {
        const ServiceCommunicatonBase::MapInstances & observers = mLoggerService.getObservers();
        for (auto pos = observers.firstPosition(); observers.isValidPosition(pos); pos = observers.nextPosition(pos))
        {
            const ITEM_ID & key{ observers.keyAtPosition(pos) };
            RemoteMessage msg{ msgReceived.clone(source, key) };
            mLoggerService.sendMessage(msg);
        }
    }
}
