/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/NetTcpLogger.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, TCP/IP logging object to log message into the
 *              remote object.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/logging/private/NetTcpLogger.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SyncObjects.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeController.hpp"

#if AREG_LOGS

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
            mRingStack.release();
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
    mRingStack.release();
    onServiceExit();
    unregisterForServiceClientCommands();
}

void NetTcpLogger::logMessage(const NELogging::sLogMessage& logMessage)
{
    if (mIsEnabled)
    {
        if (mChannel.isValid() && isConnectState())
        {
            sendMessage(NELogging::createLogMessage(logMessage, NELogging::eLogDataType::LogDataRemote, mChannel.getCookie()), Event::eEventPriority::EventPriorityNormal);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(NELogging::createLogMessage(logMessage, NELogging::eLogDataType::LogDataRemote, mChannel.getCookie()));
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
        RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.setSource(cookie);
        reinterpret_cast<NELogging::sLogMessage*>(msgLog.getBuffer())->logCookie = cookie;
        sendMessage(msgLog, Event::eEventPriority::EventPriorityNormal);
    }
}

void NetTcpLogger::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mIsEnabled = false;
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::lostRemoteServiceChannel(const Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::failedSendMessage(const RemoteMessage & msgFailed, Socket & /* whichTarget */)
{
    ASSERT(mIsEnabled);
    if (mLogConfiguration.getStackSize() > 0)
    {
        mRingStack.push(msgFailed);
    }

    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
}

void NetTcpLogger::failedReceiveMessage(Socket & /* whichSource */)
{
    sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
}

void NetTcpLogger::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
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
            serviceConnectionEvent(msgReceived);
            break;

        case NEService::eFuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                NELogging::sScopeInfo scopeInfo{};
                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeInfo;
                    LogManager::updateScopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                }

                if (scopeCount != 0)
                {
                    const NELogging::ScopeList& scopes{ static_cast<const NELogging::ScopeList&>(mScopeController.getScopeList()) };
                    sendMessage(NELogging::messageScopesUpdated(mChannel.getCookie(), NEService::COOKIE_LOGGER, scopes));
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceLogQueryScopes:
            {
                const NELogging::ScopeList & scopes{ static_cast<const NELogging::ScopeList &>(mScopeController.getScopeList()) };
                const ITEM_ID & targetId{ msgReceived.getSource() };
                sendMessage(NELogging::messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
            }
            break;

        case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::saveLogConfig())
            {
                sendMessage(NELogging::messageConfigurationSaved());
            }
            break;

        case NEService::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
        case NEService::eFuncIdRange::ServiceLastId:                    // fall through
        case NEService::eFuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::eFuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::eFuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::eFuncIdRange::SystemServiceConnect:             // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::eFuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::eFuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::eFuncIdRange::RequestRegisterService:           // fall through
        case NEService::eFuncIdRange::ComponentCleanup:                 // fall through
        case NEService::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
        case NEService::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
        case NEService::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
        case NEService::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::eFuncIdRange::ServiceLogMessage:                // fall through
        case NEService::eFuncIdRange::AttributeLastId:                  // fall through
        case NEService::eFuncIdRange::AttributeFirstId:                 // fall through
        case NEService::eFuncIdRange::ResponseLastId:                   // fall through
        case NEService::eFuncIdRange::ResponseFirstId:                  // fall through
        case NEService::eFuncIdRange::RequestLastId:                    // fall through
        case NEService::eFuncIdRange::RequestFirstId:                   // fall through
        case NEService::eFuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

#endif  // AREG_LOGS
