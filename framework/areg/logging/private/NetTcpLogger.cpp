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

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, areg::DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , areg::ServiceClientConnectionBase   ( areg::COOKIE_LOGGER
                                    , areg::RemoteServiceKind::Logger
                                    , static_cast<uint32_t>(areg::ConnectionType::Tcpip)
                                    , areg::MessageSource::SourceClient
                                    , static_cast<ConnectionConsumer &>(self())
                                    , static_cast<RemoteMessageHandler &>(self())
                                    , dispatchThread
                                    , NetTcpLogger::PREFIX_THREAD)
    , ConnectionConsumer   ( )
    , RemoteMessageHandler        ( )

    , mScopeController  ( scopeController )
    , mIsEnabled        ( false )
    , mRingStack        ( 0, areg::OverlapPolicy::Shift )
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

            areg::String host{ mLogConfiguration.getRemoteTcpAddress()};
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

void NetTcpLogger::logMessage(const areg::LogEntry& logMessage)
{
    if (mIsEnabled)
    {
        if (mChannel.isValid() && isConnectState())
        {
            sendMessage(areg::createLogMessage(logMessage, areg::LogDataType::Remote, mChannel.getCookie()), areg::Event::EventPriority::NormalPrio);
        }
        else if (mRingStack.capacity() != 0)
        {
            mRingStack.push(areg::createLogMessage(logMessage, areg::LogDataType::Remote, mChannel.getCookie()));
        }
    }
}

bool NetTcpLogger::isLoggerOpened() const
{
    Lock lock( mLock );
    return isConnectedState();
}

void NetTcpLogger::connectedRemoteServiceChannel(const areg::Channel & channel)
{
    ASSERT(channel.isValid());
    ASSERT(channel.getCookie() >= areg::COOKIE_REMOTE_SERVICE);
    ASSERT(mChannel.isValid());

    mIsEnabled = true;
    const ITEM_ID& cookie = channel.getCookie();
    while (mRingStack.isEmpty() == false)
    {
        areg::RemoteMessage msgLog{ mRingStack.pop() };
        msgLog.setSource(cookie);
        reinterpret_cast<areg::LogEntry*>(msgLog.getBuffer())->logCookie = cookie;
        sendMessage(msgLog, areg::Event::EventPriority::NormalPrio);
    }
}

void NetTcpLogger::disconnectedRemoteServiceChannel(const areg::Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mIsEnabled = false;
    mClientConnection.setCookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::lostRemoteServiceChannel(const areg::Channel & /* channel */)
{
    ASSERT(mChannel.isValid() == false);
    mClientConnection.setCookie(areg::COOKIE_UNKNOWN);
}

void NetTcpLogger::failedSendMessage(const areg::RemoteMessage & msgFailed, areg::Socket & /* whichTarget */)
{
    ASSERT(mIsEnabled);
    if (mLogConfiguration.getStackSize() > 0)
    {
        mRingStack.push(msgFailed);
    }

    sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failedReceiveMessage(areg::Socket & /* whichSource */)
{
    sendCommand(areg::ServiceEventData::ServiceCommand::CMD_ServiceLost);
}

void NetTcpLogger::failedProcessMessage(const areg::RemoteMessage & /* msgUnprocessed */)
{
}

void NetTcpLogger::processReceivedMessage(const areg::RemoteMessage & msgReceived, areg::Socket & whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        areg::FuncIdRange msgId = static_cast<areg::FuncIdRange>(msgReceived.getMessageId());
        switch (msgId)
        {
        case areg::FuncIdRange::SystemServiceNotifyConnection:
            serviceConnectionEvent(msgReceived);
            break;

        case areg::FuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                areg::ScopeEntry scopeInfo{};
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

        case areg::FuncIdRange::ServiceLogQueryScopes:
            {
                const areg::ScopeList & scopes{ static_cast<const areg::ScopeList &>(mScopeController.getScopeList()) };
                const ITEM_ID & targetId{ msgReceived.getSource() };
                sendMessage(areg::messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
            }
            break;

        case areg::FuncIdRange::ServiceSaveLogConfiguration:
            if (LogManager::saveLogConfig())
            {
                sendMessage(areg::messageConfigurationSaved());
            }
            break;

        case areg::FuncIdRange::SystemServiceNotifyRegister:      // fall through
        case areg::FuncIdRange::ServiceLastId:                    // fall through
        case areg::FuncIdRange::SystemServiceQueryInstances:      // fall through
        case areg::FuncIdRange::SystemServiceRequestRegister:     // fall through
        case areg::FuncIdRange::SystemServiceDisconnect:          // fall through
        case areg::FuncIdRange::SystemServiceConnect:             // fall through
        case areg::FuncIdRange::ResponseServiceProviderConnection:// fall through
        case areg::FuncIdRange::RequestServiceProviderConnection: // fall through
        case areg::FuncIdRange::ResponseServiceProviderVersion:   // fall through
        case areg::FuncIdRange::RequestServiceProviderVersion:    // fall through
        case areg::FuncIdRange::RequestRegisterService:           // fall through
        case areg::FuncIdRange::ComponentCleanup:                 // fall through
        case areg::FuncIdRange::SystemServiceNotifyInstances:     // fall through
        case areg::FuncIdRange::ServiceLogRegisterScopes:         // fall through
        case areg::FuncIdRange::ServiceLogScopesUpdated:          // fall through
        case areg::FuncIdRange::ServiceLogConfigurationSaved:     // fall through
        case areg::FuncIdRange::ServiceLogMessage:                // fall through
        case areg::FuncIdRange::AttributeLastId:                  // fall through
        case areg::FuncIdRange::AttributeFirstId:                 // fall through
        case areg::FuncIdRange::ResponseLastId:                   // fall through
        case areg::FuncIdRange::ResponseFirstId:                  // fall through
        case areg::FuncIdRange::RequestLastId:                    // fall through
        case areg::FuncIdRange::RequestFirstId:                   // fall through
        case areg::FuncIdRange::EmptyFunctionId:                  // fall through
        default:
            ASSERT(false);
        }
    }
}

#endif  // AREG_LOGS
