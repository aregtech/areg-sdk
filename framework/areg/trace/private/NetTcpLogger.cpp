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
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, TCP/IP Logger object to log message into the
 *              remote object.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/trace/private/NetTcpLogger.hpp"
#include "areg/trace/private/TraceManager.hpp"
#include "areg/trace/private/ScopeController.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SynchObjects.hpp"

NetTcpLogger::NetTcpLogger(LogConfiguration & logConfig, ScopeController & scopeController, DispatcherThread & dispatchThread)
    : LoggerBase                    (logConfig)
    , ServiceClientConnectionBase   ( NEService::COOKIE_LOGGER, static_cast<IEServiceConnectionConsumer &>(self()), static_cast<IERemoteMessageHandler &>(self()), dispatchThread)
    , IEServiceConnectionConsumer   ( )
    , IERemoteMessageHandler        ( )

    , mScopeController  (scopeController)
    , mIsEnabled        ( false )
    , mRingStack        ( NetTcpLogger::RING_STACK_MAX_SIZE, NECommon::eRingOverlap::ShiftOnOverlap )
{
}

bool NetTcpLogger::openLogger(void)
{
    Lock lock( mLock );
    bool result{ false };

    if (mClientConnection.isValid() == false)
    {
        mIsEnabled = false;
        if (mTracerConfiguration.isNetLoggingEnabled())
        {
            const LogConfiguration & logConfig = getTraceConfiguration();
            const TraceProperty & propTcpHost = logConfig.getRemoteTcpHost();
            const TraceProperty & propTcpPort = logConfig.getRemoteTcpPort();
            if (propTcpHost.isValid() && propTcpPort.isValid())
            {
                mIsEnabled = true;
                String host(static_cast<const String &>(propTcpHost.getValue()));
                uint16_t port(static_cast<const uint16_t>(propTcpPort.getValue()));
                applyServiceConnectionData(host, port);
                mLock.unlock();
                result = connectServiceHost();
            }
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
    disconnectServiceHost();
    mRingStack.clear();
}

void NetTcpLogger::logMessage(const NETrace::sLogMessage& logMessage)
{
    if (mIsEnabled)
    {
        RemoteMessage log(NETrace::messageLog(logMessage));

        if (isConnectedState())
        {
            sendMessage(log, Event::eEventPriority::EventPriorityNormal);
        }
        else
        {
            mRingStack.pushLast(log);
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
    ASSERT(channel.getSource() == NEService::COOKIE_LOGGER);
    ASSERT(channel.getCookie() >= NEService::COOKIE_REMOTE_SERVICE);

    mIsEnabled = true;
    mChannel = channel;
    mClientConnection.setCookie(channel.getCookie());

    while (mRingStack.isEmpty() == false)
    {
        RemoteMessage msg{ mRingStack.popFirst() };
        msg.setSource(channel.getCookie());
        sendMessage(msg, Event::eEventPriority::EventPriorityNormal);
    }
}

void NetTcpLogger::disconnectedRemoteServiceChannel(const Channel & channel)
{
    mIsEnabled = false;
    mChannel.invalidate();
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::lostRemoteServiceChannel(const Channel & channel)
{
    mChannel.invalidate();
    mClientConnection.setCookie(NEService::COOKIE_UNKNOWN);
}

void NetTcpLogger::failedSendMessage(const RemoteMessage & msgFailed, Socket & whichTarget)
{
}

void NetTcpLogger::failedReceiveMessage(Socket & whichSource)
{
}

void NetTcpLogger::failedProcessMessage(const RemoteMessage & msgUnprocessed)
{
}

void NetTcpLogger::processReceivedMessage(const RemoteMessage & msgReceived, Socket & whichSource)
{
    if (msgReceived.isValid() && whichSource.isValid())
    {
        ASSERT(msgReceived.getTarget() == mChannel.getCookie());

        const NEService::eFuncIdRange msgId { static_cast<NEService::eFuncIdRange>(msgReceived.getMessageId()) };
        ITEM_ID target{ NEService::COOKIE_ANY };
        msgReceived >> target;

        switch (msgId)
        {
        case NEService::eFuncIdRange::ServiceLogUpdateScopes:
            {
                uint32_t scopeCount{ 0 };
                String scopeName;
                uint32_t scopeId{ 0 };
                uint32_t scopePrio{ 0 };

                msgReceived >> scopeCount;
                for ( uint32_t i = 0; i < scopeCount; ++ i)
                {
                    msgReceived >> scopeName;
                    msgReceived >> scopeId;
                    msgReceived >> scopePrio;

                    TraceManager::updateScopes(scopeName, scopeId, scopePrio);
                }
            }
            break;

        case NEService::eFuncIdRange::ServiceLogQueryScopes:
            {
                const TraceScopeMap & scopes{ mScopeController.getScopeList() };
                const uint32_t scopeCount{ scopes.getSize() };
                const ITEM_ID & targetId{ msgReceived.getSource() };

                RemoteMessage msgScopeBegin{ NETrace::messageRegisterScopesStart(targetId, scopeCount) };
                sendMessage(msgScopeBegin);
                uint32_t scopesSent{ 0 };
                NETrace::SCOPEPOS scopePos = scopes.invalidPosition();
                while (scopeCount > scopesSent)
                {
                    RemoteMessage msgScope{ NETrace::messageRegisterScopes(targetId, static_cast<const NETrace::ScopeList &>(scopes), scopePos, NETrace::SCOPE_ENTRIES_MAX_COUNT) };
                    if (msgScope.isValid())
                    {
                        sendMessage(msgScope);
                    }

                    scopesSent += NETrace::SCOPE_ENTRIES_MAX_COUNT;
                }

                RemoteMessage msgScopeEnd{ NETrace::messageRegisterScopesEnd(targetId) };
                sendMessage(msgScopeEnd);
            }
            break;

        case NEService::eFuncIdRange::ServiceSaveLogConfiguration:
            TraceManager::saveLogConfig(nullptr);
            break;

        default:
            ASSERT(false);
        }
    }
}
