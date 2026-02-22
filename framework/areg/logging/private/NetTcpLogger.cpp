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
    , ServiceClientConnectionBase   ( NEService::COOKIE_LOGGER
                                    , NERemoteService::RemoteServiceKind::Logger
                                    , static_cast<uint32_t>(NERemoteService::ConnectionType::Tcpip)
                                    , NEService::MessageSource::SourceClient
                                    , static_cast<ConnectionConsumer &>(self())
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer   ( )
    , RemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, NECommon::OverlapPolicy::Shift )
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

void NetTcpLogger::logMessage(const NELogging::LogEntry& logMessage)
{
    if (mIsEnabled)
    {
        if (mChannel.isValid() && isConnectState())
        {
            sendMessage(NELogging::createLogMessage(logMessage, NELogging::LogDataType::Remote, mChannel.getCookie()), Event::EventPriority::NormalPrio);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(NELogging::createLogMessage(logMessage, NELogging::LogDataType::Remote, mChannel.getCookie()));
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
    ASSERT(channel.getCookie() >= NEService::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.isValid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.getCookie();
    while (mRingStack.isEmpty() == false)
    {
        RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.setSource(cookie);
        reinterpret_cast<NELogging::LogEntry*>(msgLog.getBuffer())->logCookie = cookie;
        sendMessage(msgLog, Event::EventPriority::NormalPrio);
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

    sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failedReceiveMessage(Socket & /* whichSource */)
{
    sendCommand(ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failedProcessMessage(const RemoteMessage & /* msgUnprocessed */)
{
}

void NetTcpLogger::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        NEService::FuncIdRange msgId = static_cast<NEService::FuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case NEService::FuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case NEService::FuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                NELogging::ScopeEntry scopeInfo{};
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

        case NEService::FuncIdRange::ServiceLogQueryScopes:
            {
                const NELogging::ScopeList & scopes{ static_cast<const NELogging::ScopeList &>(mScopeController.getScopeList()) };
                const ITEM_ID & targetId{ msgReceived.getSource() };
                sendMessage(NELogging::messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
            }
            break;

        case NEService::FuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::saveLogConfig())
            {
                sendMessage(NELogging::messageConfigurationSaved());
            }
            break;

        case NEService::FuncIdRange::SystemServiceNotifyRegister:      // fall through
        case NEService::FuncIdRange::ServiceLastId:                    // fall through
        case NEService::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case NEService::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case NEService::FuncIdRange::SystemServiceDisconnect:          // fall through
        case NEService::FuncIdRange::SystemServiceConnect:             // fall through
        case NEService::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case NEService::FuncIdRange::RequestServiceProviderConnection: // fall through
        case NEService::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case NEService::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case NEService::FuncIdRange::RequestRegisterService:           // fall through
        case NEService::FuncIdRange::ComponentCleanup:                 // fall through
        case NEService::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case NEService::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case NEService::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case NEService::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case NEService::FuncIdRange::ServiceLogMessage:                // fall through
        case NEService::FuncIdRange::AttributeLastId:                  // fall through
        case NEService::FuncIdRange::AttributeFirstId:                 // fall through
        case NEService::FuncIdRange::ResponseLastId:                   // fall through
        case NEService::FuncIdRange::ResponseFirstId:                  // fall through
        case NEService::FuncIdRange::RequestLastId:                    // fall through
        case NEService::FuncIdRange::RequestFirstId:                   // fall through
        case NEService::FuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

#endif  // AREG_LOGS
