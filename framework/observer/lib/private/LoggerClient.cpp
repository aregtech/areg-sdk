/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/private/LoggerClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The logger service connection client.
 ************************************************************************/

#include "observer/lib/private/LoggerClient.hpp"

#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/trace/NETrace.hpp"
#include "observer/lib/LogObserverApi.h"

#define IS_VALID(callback)  ((mCallbacks != nullptr) && ((callback) != nullptr))

LoggerClient& LoggerClient::getInstance(void)
{
    static LoggerClient _instance;
    return _instance;
}

LoggerClient::LoggerClient(void)
    : ServiceClientConnectionBase( TargetID
                                 , ServiceType
                                 , static_cast<uint32_t>(ConnectType)
                                 , SourceType
                                 , static_cast<IEServiceConnectionConsumer &>(self())
                                 , static_cast<IERemoteMessageHandler &>(self())
                                 , static_cast<DispatcherThread &>(self())
                                 , ThreadPrefix)
    , DispatcherThread           ( ThreadName )
    , IEServiceConnectionConsumer( )
    , IERemoteMessageHandler     ( )

    , mCallbacks                 ( nullptr )
    , mMessageProcessor          ( self() )
{
}

bool LoggerClient::startLoggerClient(const String & address /*= String::EmptyString*/, uint16_t portNr /*= NESocket::InvalidPort*/)
{
    Lock lock(mLock);
    if ((address.isEmpty() == false) && (portNr != NESocket::InvalidPort))
    {
        applyServiceConnectionData(address, portNr);
    }
    else
    {
        ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
        if (config.isConfigured())
        {
            applyServiceConnectionData(config.getConnectionAddress(), config.getConnectionPort());
        }
    }

    return connectServiceHost();
}

void LoggerClient::stopLoggerClient(void)
{
    Lock lock(mLock);
    disconnectServiceHost();
}

void LoggerClient::setCallbacks(const sObserverEvents* callbacks)
{
    Lock lock(mLock);
    mCallbacks = callbacks;
}

const NESocket::SocketAddress& LoggerClient::getAddress(void) const
{
    Lock lock(mLock);
    return mClientConnection.getAddress();
}

bool LoggerClient::isConfigLoggerConnectEnabled(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    return (config.isConfigured() && config.getConnectionEnableFlag());
}

String LoggerClient::getConfigLoggerAddress(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    if (config.isConfigured())
    {
        return config.getConnectionAddress();
    }
    else
    {
        return String::EmptyString;
    }
}

uint16_t LoggerClient::getConfigLoggerPort(void) const
{
    ConnectionConfiguration config(LoggerClient::ServiceType, LoggerClient::ConnectType);
    if (config.isConfigured())
    {
        return config.getConnectionPort();
    }
    else
    {
        return NESocket::InvalidPort;
    }
}

void LoggerClient::requestConnectedInstances(void)
{
    if (mChannel.getCookie() != NEService::COOKIE_UNKNOWN)
    {
        sendMessage(NETrace::messageQueryInstances(mChannel.getCookie(), LoggerClient::TargetID));
    }
}

void LoggerClient::requestScopes(const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        sendMessage(NETrace::messageQueryScopes(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target));
    }
}

void LoggerClient::requestChangeScopePrio(const NETrace::ScopeNames & scopes, const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        sendMessage(NETrace::messageUpdateScopes(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target, scopes));
    }
}

void LoggerClient::requestSaveConfiguration(const ITEM_ID& target /*= NEService::COOKIE_ANY*/)
{
    if ((mChannel.getCookie() != NEService::COOKIE_UNKNOWN) && (target != NEService::COOKIE_UNKNOWN))
    {
        sendMessage(NETrace::messageSaveConfiguration(mChannel.getCookie(), target == NEService::COOKIE_ANY ? LoggerClient::TargetID : target));
    }
}

void LoggerClient::readyForEvents(bool isReady)
{
    if (isReady)
    {
        registerForServiceClientCommands();
        DispatcherThread::readyForEvents(true);
        setConnectionState(ServiceClientConnectionBase::eConnectionState::DisconnectState);
    }
    else
    {
        DispatcherThread::readyForEvents(false);
        setConnectionState(ServiceClientConnectionBase::eConnectionState::ConnectionStopped);
        unregisterForServiceClientCommands();
    }
}

bool LoggerClient::connectServiceHost(void)
{
    bool result{ false };
    if (isRunning() == false)
    {
        if (createThread(NECommon::WAIT_INFINITE) && waitForDispatcherStart(NECommon::WAIT_INFINITE))
        {
            result = ServiceClientConnectionBase::connectServiceHost();
        }
        else
        {
            shutdownThread(NECommon::WAIT_INFINITE);
        }
    }
    else
    {
        result = mClientConnection.isValid();
    }

    return result;
}

void LoggerClient::disconnectServiceHost(void)
{
    if (isRunning())
    {
        ServiceClientConnectionBase::disconnectServiceHost();
        completionWait(NECommon::WAIT_INFINITE);
        shutdownThread(NECommon::DO_NOT_WAIT);
    }
}

void LoggerClient::onServiceExit(void)
{
    ServiceClientConnectionBase::onServiceExit();
    triggerExit();
}

void LoggerClient::connectedRemoteServiceChannel(const Channel& channel)
{
    Lock lock(mLock);
    if (IS_VALID(mCallbacks->evtServiceConnected))
    {
        const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
        mCallbacks->evtServiceConnected(true, addr.getHostAddress().getString(), addr.getHostPort());

        sendMessage(NETrace::messageQueryInstances(channel.getCookie(), LoggerClient::TargetID));
    }
}

void LoggerClient::disconnectedRemoteServiceChannel(const Channel& channel)
{
    Lock lock(mLock);
    if (IS_VALID(mCallbacks->evtServiceConnected))
    {
        const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
        mCallbacks->evtServiceConnected(false, addr.getHostAddress().getString(), addr.getHostPort());
    }
}

void LoggerClient::lostRemoteServiceChannel(const Channel& channel)
{
    Lock lock(mLock);
    if (IS_VALID(mCallbacks->evtServiceConnected))
    {
        const NESocket::SocketAddress& addr{ mClientConnection.getAddress() };
        mCallbacks->evtServiceConnected(false, addr.getHostAddress().getString(), addr.getHostPort());
    }
}

void LoggerClient::failedSendMessage(const RemoteMessage& msgFailed, Socket& whichTarget)
{
    Lock lock(mLock);
    if (IS_VALID(mCallbacks->evtMessagingFailed))
    {
        mCallbacks->evtMessagingFailed();
    }
}

void LoggerClient::failedReceiveMessage(Socket& whichSource)
{
    Lock lock(mLock);
    if (IS_VALID(mCallbacks->evtMessagingFailed))
    {
        mCallbacks->evtMessagingFailed();
    }
}

void LoggerClient::failedProcessMessage(const RemoteMessage& msgUnprocessed)
{
}

void LoggerClient::processReceivedMessage(const RemoteMessage& msgReceived, Socket& whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case NEService::eFuncIdRange::SystemServiceNotifyConnection:
            mMessageProcessor.notifyServiceConnection(msgReceived);
            break;

        case NEService::eFuncIdRange::SystemServiceNotifyInstances:
            mMessageProcessor.notifyConnectedClients(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogRegisterScopes:
            mMessageProcessor.notifyLogRegisterScopes(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogMessage:
            mMessageProcessor.notifyLogMessage(msgReceived);
            break;

        default:
            ASSERT(false);
        }
    }
}
