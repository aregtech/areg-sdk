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

namespace areg
{

    #if AREG_LOGS

    NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
        : LoggerBase                    (logConfig)
        , ServiceClientConnectionBase   ( COOKIE_LOGGER
                                        , RemoteServiceKind::Logger
                                        , static_cast<uint32_t>(ConnectionType::Tcpip)
                                        , MessageSource::SourceClient
                                        , static_cast<ConnectionConsumer &>(self())
                                        , static_cast<RemoteMessageHandler &>(self())
                                        , dispatchThread
                                        , NetTcpLogger::PREFIX_THREAD)
        , ConnectionConsumer   ( )
        , RemoteMessageHandler        ( )

        , mScopeController  ( scopeController )
        , mIsEnabled        ( false )
        , mRingStack        ( 0, OverlapPolicy::Shift )
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

    void NetTcpLogger::logMessage(const LogEntry& logMessage)
    {
        if (mIsEnabled)
        {
            if (mChannel.isValid() && isConnectState())
            {
                sendMessage(createLogMessage(logMessage, LogDataType::Remote, mChannel.getCookie()), Event::EventPriority::NormalPrio);
            }
            else if (mRingStack.capacity() != 0)
            {
                mRingStack.push(createLogMessage(logMessage, LogDataType::Remote, mChannel.getCookie()));
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
        ASSERT(channel.getCookie() >= COOKIE_REMOTE_SERVICE);
        ASSERT(mChannel.isValid());

        mIsEnabled = true;
        const ITEM_ID& cookie = channel.getCookie();
        while (mRingStack.isEmpty() == false)
        {
            RemoteMessage msgLog{ mRingStack.pop() };
            msgLog.setSource(cookie);
            reinterpret_cast<LogEntry*>(msgLog.getBuffer())->logCookie = cookie;
            sendMessage(msgLog, Event::EventPriority::NormalPrio);
        }
    }

    void NetTcpLogger::disconnectedRemoteServiceChannel(const Channel & /* channel */)
    {
        ASSERT(mChannel.isValid() == false);
        mIsEnabled = false;
        mClientConnection.setCookie(COOKIE_UNKNOWN);
    }

    void NetTcpLogger::lostRemoteServiceChannel(const Channel & /* channel */)
    {
        ASSERT(mChannel.isValid() == false);
        mClientConnection.setCookie(COOKIE_UNKNOWN);
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
            FuncIdRange msgId = static_cast<FuncIdRange>(msgReceived.getMessageId());
            switch (msgId)
            {
            case FuncIdRange::SystemServiceNotifyConnection:
                serviceConnectionEvent(msgReceived);
                break;

            case FuncIdRange::ServiceLogUpdateScopes:
                {
                    uint32_t scopeCount{ 0 };
                    ScopeEntry scopeInfo{};
                    msgReceived >> scopeCount;
                    for ( uint32_t i = 0; i < scopeCount; ++ i)
                    {
                        msgReceived >> scopeInfo;
                        LogManager::updateScopes(scopeInfo.scopeName, scopeInfo.scopeId, scopeInfo.scopePrio);
                    }

                    if (scopeCount != 0)
                    {
                        const ScopeList& scopes{ static_cast<const ScopeList&>(mScopeController.getScopeList()) };
                        sendMessage(messageScopesUpdated(mChannel.getCookie(), COOKIE_LOGGER, scopes));
                    }
                }
                break;

            case FuncIdRange::ServiceLogQueryScopes:
                {
                    const ScopeList & scopes{ static_cast<const ScopeList &>(mScopeController.getScopeList()) };
                    const ITEM_ID & targetId{ msgReceived.getSource() };
                    sendMessage(messageRegisterScopes(mChannel.getCookie(), targetId, scopes));
                }
                break;

            case FuncIdRange::ServiceSaveLogConfiguration:
                if (LogManager::saveLogConfig())
                {
                    sendMessage(messageConfigurationSaved());
                }
                break;

            case FuncIdRange::SystemServiceNotifyRegister:      // fall through
            case FuncIdRange::ServiceLastId:                    // fall through
            case FuncIdRange::SystemServiceQueryInstances:      // fall through
            case FuncIdRange::SystemServiceRequestRegister:     // fall through
            case FuncIdRange::SystemServiceDisconnect:          // fall through
            case FuncIdRange::SystemServiceConnect:             // fall through
            case FuncIdRange::ResponseServiceProviderConnection:// fall through
            case FuncIdRange::RequestServiceProviderConnection: // fall through
            case FuncIdRange::ResponseServiceProviderVersion:   // fall through
            case FuncIdRange::RequestServiceProviderVersion:    // fall through
            case FuncIdRange::RequestRegisterService:           // fall through
            case FuncIdRange::ComponentCleanup:                 // fall through
            case FuncIdRange::SystemServiceNotifyInstances:     // fall through
            case FuncIdRange::ServiceLogRegisterScopes:         // fall through
            case FuncIdRange::ServiceLogScopesUpdated:          // fall through
            case FuncIdRange::ServiceLogConfigurationSaved:     // fall through
            case FuncIdRange::ServiceLogMessage:                // fall through
            case FuncIdRange::AttributeLastId:                  // fall through
            case FuncIdRange::AttributeFirstId:                 // fall through
            case FuncIdRange::ResponseLastId:                   // fall through
            case FuncIdRange::ResponseFirstId:                  // fall through
            case FuncIdRange::RequestLastId:                    // fall through
            case FuncIdRange::RequestFirstId:                   // fall through
            case FuncIdRange::EmptyFunctionId:                  // fall through
            default:
                ASSERT(false);
            }
        }
    }

    #endif  // AREG_LOGS
} // namespace areg
