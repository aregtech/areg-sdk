/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/NetTcpLogger.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, TCP/IP Logger object to log message into the
 *              remote object.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/NetTcpLogger.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/trace/private/TraceManager.hpp"
#include "areg/trace/private/ScopeController.hpp"


NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( NEService::COOKIE_LOGGER
                                    , NERemoteService::eRemoteServices::ServiceLogger
                                    , static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip)
                                    , NEService::eMessageSource::MessageSourceClient
                                    , static_cast<IEServiceConnectionConsumer &>(self())
                                    , static_cast<IERemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , IEServiceConnectionConsumer   ( )
    , IERemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, NECommon::eRingOverlap::ShiftOnOverlap )
{
}

bool NetTcpLogger::openLogger(void)
{
    Lock lock( mLock );
    bool result{ false };

    if (mClientConnection.isValid() == false)
    {
        mIsEnabled = false;
        if (mLogConfiguration.isRemoteLoggingEnabled())
        {
            registerForServiceClientCommands();
            mRingStack.reserve(mLogConfiguration.getStackSize());

            String host{ mLogConfiguration.getRemoteTcpAddress()};
            uint16_t port{ mLogConfiguration.getRemoteTcpPort() };
            mIsEnabled = true;
            applyServiceConnectionData(host, port);
            mLock.unlock();
            result = connectServiceHost();
        }
        else
        {
            mRingStack.discard();
        }
    }
    else
    {
        result = isConnectState();
    }

    return result;
}

void NetTcpLogger::closeLogger(void)
{
    mRingStack.discard();
    onServiceExit();
    unregisterForServiceClientCommands();
}

void NetTcpLogger::logMessage(const NETrace::sLogMessage& logMessage)
{
    if (mIsEnabled)
    {
        if (isConnectedState())
        {
            ASSERT(mChannel.isValid());
            sendMessage(NETrace::createLogMessage(logMessage, NETrace::eLogDataType::LogDataRemote, mChannel.getCookie()), Event::eEventPriority::EventPriorityNormal);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.pushLast(NETrace::createLogMessage(logMessage, NETrace::eLogDataType::LogDataRemote, mChannel.getCookie()));
        }
    }
}

bool NetTcpLogger::isLoggerOpened(void) const
{
    Lock lock( mLock );
    return isConnectedState();
}

void NetTcpLogger::connectedRemoteServiceChannel(const Channel & channel)
{
    ASSERT(channel.isValid());
    ASSERT(channel.getCookie() >= NEService::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.isValid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.getCookie();
    while (mRingStack.isEmpty() == false)
    {
        RemoteMessage msgLog{ mRingStack.popFirst() };
        msgLog.setSource(cookie);
        reinterpret_cast<NETrace::sLogMessage*>(msgLog.getBuffer())->logCookie = cookie;
        sendMessage(msgLog, Event::eEventPriority::EventPriorityNormal);
    }
}

void NetTcpLogger::disconnectedRemoteServiceChannel(const Channel & channel)
{
    ASSERT(mChannel.isValid() == false);
    mIsEnabled = false;
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::lostRemoteServiceChannel(const Channel & channel)
{
    ASSERT(mChannel.isValid() == false);
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget)
{
    ASSERT(mIsEnabled);
    if (mLogConfiguration.getStackSize() > 0)
    {
        mRingStack.pushLast(msgFailed);
    }

    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
}

void NetTcpLogger::failedReceiveMessage(Socket & whichSource)
{
    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
}

void NetTcpLogger::failedProcessMessage(const RemoteMessage & msgUnprocessed)
{
}

void NetTcpLogger::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        NEService::eFuncIdRange msgId = static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case NEService::eFuncIdRange::SystemServiceNotifyConnection:
        {
            ITEM_ID cookie{ NEService::COOKIE_UNKNOWN };
            NEService::eServiceConnection connection{ NEService::eServiceConnection::ServiceConnectionUnknown };
            msgReceived >> cookie;
            msgReceived >> connection;

            switch (connection)
            {
            case NEService::eServiceConnection::ServiceConnected:
            case NEService::eServiceConnection::ServicePending:
                {
                    if (msgReceived.getResult() == NEMemory::MESSAGE_SUCCESS)
                    {
                        Lock lock(mLock);
                        ASSERT(cookie == msgReceived.getTarget());
                        mClientConnection.setCookie(cookie);
                        sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStarted);
                    }
                    else
                    {
                        cancelConnection();
                        sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
                    }
                }
                break;

            case NEService::eServiceConnection::ServiceRejected:
                {
                    cancelConnection();
                    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStopped);
                }
                break;

            default:
                {
                    cancelConnection();
                    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
                }
                break;
            }
        }
        break;

        case NEService::eFuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                NETrace::sScopeInfo scopeInfo{};
                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeInfo;
                    TraceManager::updateScopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceLogQueryScopes:
            {
                const NETrace::ScopeList & scopes{ static_cast<const NETrace::ScopeList &>(mScopeController.getScopeList()) };
                const ITEM_ID & targetId{ msgReceived.getSource() };
                sendMessage(NETrace::messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
            }
            break;

        case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
            Application::getConfigManager().saveConfig();
            break;

        default:
            ASSERT(false);
        }
    }
}
