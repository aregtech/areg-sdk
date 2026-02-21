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
#include "areg/base/SyncPrimitives.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/logging/private/LogManager.hpp"
#include "areg/logging/private/ScopeController.hpp"

#if AREG_LOGS

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( areg::COOKIE_LOGGER
                                    , areg::eRemoteServices::ServiceLogger
                                    , static_cast<uint32_t>(areg::eConnectionTypes::ConnectTcpip)
                                    , areg::eMessageSource::MessageSourceClient
                                    , static_cast<ConnectionConsumer &>(self())
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer   ( )
    , RemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, areg::eRingOverlap::ShiftOnOverlap )
{
}

bool NetTcpLogger::openLogger()
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

void NetTcpLogger::closeLogger()
{
    mRingStack.release();
    onServiceExit();
    unregisterForServiceClientCommands();
}

void NetTcpLogger::logMessage(const areg::sLogMessage& logMessage)
{
    if (mIsEnabled)
    {
        if (mChannel.isValid() && isConnectState())
        {
            sendMessage(areg::createLogMessage(logMessage, areg::eLogDataType::LogDataRemote, mChannel.getCookie()), Event::eEventPriority::EventPriorityNormal);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(areg::createLogMessage(logMessage, areg::eLogDataType::LogDataRemote, mChannel.getCookie()));
        }
    }
}

bool NetTcpLogger::isLoggerOpened() const
{
    Lock lock( mLock );
    return isConnectedState();
}

void NetTcpLogger::connectedRemoteServiceChannel(const Channel & channel)
{
    ASSERT(channel.isValid());
    ASSERT(channel.getCookie() >= areg::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.isValid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.getCookie();
    while (mRingStack.isEmpty() == false)
    {
        RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.setSource(cookie);
        reinterpret_cast<areg::sLogMessage*>(msgLog.getBuffer())->logCookie = cookie;
        sendMessage(msgLog, Event::eEventPriority::EventPriorityNormal);
    }
}

void NetTcpLogger::disconnectedRemoteServiceChannel(const Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mIsEnabled = false;
    mClientConnection.setCookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::lostRemoteServiceChannel(const Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mClientConnection.setCookie(areg::COOKIE_UNKNOWN);
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
        areg::eFuncIdRange msgId = static_cast<areg::eFuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case areg::eFuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case areg::eFuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                areg::sScopeInfo scopeInfo{};
                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeInfo;
                    LogManager::updateScopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                }

                if (scopeCount != 0)
                {
                    const areg::ScopeList& scopes{ static_cast<const areg::ScopeList&>(mScopeController.getScopeList()) };
                    sendMessage(areg::messageScopesUpdated(mChannel.getCookie(), areg::COOKIE_LOGGER, scopes));
                }
            }
            break;

        case areg::eFuncIdRange::ServiceLogQueryScopes:
            {
                const areg::ScopeList & scopes{ static_cast<const areg::ScopeList &>(mScopeController.getScopeList()) };
                const ITEM_ID & targetId{ msgReceived.getSource() };
                sendMessage(areg::messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
            }
            break;

        case areg::eFuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::saveLogConfig())
            {
                sendMessage(areg::messageConfigurationSaved());
            }
            break;

        case areg::eFuncIdRange::SystemServiceNotifyRegister:      // fall through
        case areg::eFuncIdRange::ServiceLastId:                    // fall through
        case areg::eFuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::eFuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::eFuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::eFuncIdRange::SystemServiceConnect:             // fall through
        case areg::eFuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::eFuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::eFuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::eFuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::eFuncIdRange::RequestRegisterService:           // fall through
        case areg::eFuncIdRange::ComponentCleanup:                 // fall through
        case areg::eFuncIdRange::SystemServiceNotifyInstances:     // fall through
        case areg::eFuncIdRange::ServiceLogRegisterScopes:         // fall through
        case areg::eFuncIdRange::ServiceLogScopesUpdated:          // fall through
        case areg::eFuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::eFuncIdRange::ServiceLogMessage:                // fall through
        case areg::eFuncIdRange::AttributeLastId:                  // fall through
        case areg::eFuncIdRange::AttributeFirstId:                 // fall through
        case areg::eFuncIdRange::ResponseLastId:                   // fall through
        case areg::eFuncIdRange::ResponseFirstId:                  // fall through
        case areg::eFuncIdRange::RequestLastId:                    // fall through
        case areg::eFuncIdRange::RequestFirstId:                   // fall through
        case areg::eFuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

#endif  // AREG_LOGS
