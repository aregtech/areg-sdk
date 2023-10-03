/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/service/private/LoggerServerService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, logger connection service
 ************************************************************************/
#include "logger/service/LoggerServerService.hpp"

#include "areg/ipc/NEConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageReceived);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageSend);

DEF_TRACE_SCOPE(logger_service_LoggerServerService_startConnection);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_restartConnection);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_stopConnection);

DEF_TRACE_SCOPE(logger_service_LoggerServerService_failedSendMessage);
DEF_TRACE_SCOPE(logger_service_LoggerServerService_failedReceiveMessage);

//////////////////////////////////////////////////////////////////////////
// LoggerServerService class implementation
//////////////////////////////////////////////////////////////////////////

LoggerServerService::LoggerServerService( void )
    : ServiceCommunicatonBase   ( NEService::COOKIE_LOGGER, NEConnection::SERVER_DISPATCH_MESSAGE_THREAD, ServiceCommunicatonBase::eConnectionBehavior::DefaultAccept )
    , mLoggerProcessor          ( self() )
    , mObservers                ( )
{
}

void LoggerServerService::addInstance(const ITEM_ID & cookie, const sConnectedInstance & instance)
{
    Lock lock(mLock);
    ServiceCommunicatonBase::addInstance(cookie, instance);
    if (instance.ciSource != NEService::eMessageSource::MessageSourceObserver)
    {
        if (mObservers.isEmpty() == false)
        {
            mLoggerProcessor.notifyInstances();
        }
    }
    else
    {
        mObservers.addIfUnique(cookie, instance);
    }
}

void LoggerServerService::removeInstance(const ITEM_ID & cookie)
{
    Lock lock(mLock);
    sConnectedInstance instance;
    bool exists{ mInstanceMap.find(cookie, instance) };
    ServiceCommunicatonBase::removeInstance(cookie);
    if (instance.ciSource != NEService::eMessageSource::MessageSourceObserver)
    {
        if (mObservers.isEmpty() == false)
        {
            mLoggerProcessor.notifyInstances();
        }
    }
    else
    {
        mObservers.removeAt(cookie);
    }
}

void LoggerServerService::removeAllInstances(void)
{
    Lock lock(mLock);
    ServiceCommunicatonBase::removeAllInstances();
    for (auto pos = mObservers.firstPosition(); mObservers.isInvalidPosition(pos); pos = mObservers.nextPosition(pos))
    {
        mLoggerProcessor.notifyTargetInstances(mObservers.keyAtPosition(pos));
    }

    mObservers.clear();
}

void LoggerServerService::onServiceMessageReceived(const RemoteMessage &msgReceived)
{
    TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageReceived);

    ASSERT( msgReceived.isValid() );
    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgReceived.getMessageId() );
    const ITEM_ID & source = msgReceived.getSource();

    TRACE_DBG("Processing received valid message [ %s ] of id [ 0x%X ] from source [ %u ] to target [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(source)
                    , static_cast<uint32_t>(msgReceived.getTarget()));

    switch (msgId)
    {
    case NEService::eFuncIdRange::SystemServiceQueryInstances:
        mLoggerProcessor.queryInstances(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogRegisterScopes:
        mLoggerProcessor.registerScopes(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogUpdateScopes:
        mLoggerProcessor.updateScopes(msgReceived);
        break;

    case NEService::eFuncIdRange::ServiceLogQueryScopes:
        mLoggerProcessor.queryScopes(msgReceived);

    case NEService::eFuncIdRange::ServiceLogMessage:
        mLoggerProcessor.logMessage(msgReceived);
        break;

    case NEService::eFuncIdRange::RequestServiceProviderVersion:
    case NEService::eFuncIdRange::ResponseServiceProviderVersion:
    case NEService::eFuncIdRange::RequestServiceProviderConnection:
    case NEService::eFuncIdRange::ResponseServiceProviderConnection:
    case NEService::eFuncIdRange::SystemServiceConnect:
    case NEService::eFuncIdRange::SystemServiceDisconnect:
    case NEService::eFuncIdRange::SystemServiceNotifyConnection:
    case NEService::eFuncIdRange::SystemServiceNotifyInstances:
    case NEService::eFuncIdRange::RequestRegisterService:
    case NEService::eFuncIdRange::SystemServiceRequestRegister:
    case NEService::eFuncIdRange::SystemServiceNotifyRegister:
    default:
        TRACE_ERR("Unexpected logger service message!");
        ASSERT(false);
        break;
    }
}

void LoggerServerService::onServiceMessageSend(const RemoteMessage &msgSend)
{
    TRACE_SCOPE(logger_service_LoggerServerService_onServiceMessageSend);

    NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>( msgSend.getMessageId() );
    TRACE_DBG("Sending message [ %s ] of id [ 0x%X ] is going to send to target [ %u ] from source [ %u ]"
                    , NEService::getString(msgId)
                    , static_cast<uint32_t>(msgId)
                    , static_cast<uint32_t>(msgSend.getTarget())
                    , static_cast<uint32_t>(msgSend.getSource()));

    if ( NEService::isExecutableId( static_cast<uint32_t>(msgId)) )
    {
        if ( msgSend.getTarget( ) != NEService::TARGET_UNKNOWN )
        {
            sendMessage( msgSend );
        }
    }
    else
    {
        TRACE_ERR("The message [ %u ] is neither executable, nor router notification. Ignoring sending message", static_cast<uint32_t>(msgId));
        ASSERT(false);
    }
}

void LoggerServerService::connectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LoggerServerService::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{

}

void LoggerServerService::lostRemoteServiceChannel(const Channel & /* channel */)
{
}

void LoggerServerService::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{
}

void LoggerServerService::onServiceConnectionStarted( void )
{
}

void LoggerServerService::onServiceConnectionStopped( void )
{
}

void LoggerServerService::onServiceConnectionLost( void )
{
}
