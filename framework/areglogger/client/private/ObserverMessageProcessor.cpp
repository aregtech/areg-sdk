/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/ObserverMessageProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/private/ObserverMessageProcessor.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/RemoteMessage.hpp"
#include "areg/component/NEService.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/logging/LogScope.hpp"

#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/private/LoggerClient.hpp"

ObserverMessageProcessor::ObserverMessageProcessor(LoggerClient& loggerClient)
    : mLoggerClient (loggerClient)
{
}

void ObserverMessageProcessor::notifyServiceConnection(const RemoteMessage& msgReceived)
{
    mLoggerClient.serviceConnectionEvent(msgReceived);
}

void ObserverMessageProcessor::notifyConnectedClients(const RemoteMessage& msgReceived)
{
    FuncInstancesConnect    evtInstConnected{ nullptr };
    FuncInstancesDisconnect evtInstDisconnected{ nullptr };

    sLogInstance* listConnect{ nullptr };
    ITEM_ID* listDisconnect{ nullptr };
    NERemoteService::eRemoteConnection remConnect{ NERemoteService::eRemoteConnection::RemoteDisconnected };
    uint32_t count{ 0 };
    DateTime now(DateTime::getNow());

    do
    {
        Lock lock(mLoggerClient.mLock);
        msgReceived >> remConnect;
        msgReceived >> count;

        if (remConnect == NERemoteService::eRemoteConnection::RemoteConnected)
        {
            evtInstConnected = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtInstConnected : nullptr;
            listConnect = count != 0 ? new sLogInstance[count] : nullptr;
            for (uint32_t i = 0; i < count; ++i)
            {
                NEService::sServiceConnectedInstance instance{};
                msgReceived >> instance;
                auto added = mLoggerClient.mInstances.addIfUnique(instance.ciCookie, instance, false);
                if (added.second)
                {
                    mLoggerClient.mLogDatabase.logInstanceConnected(instance, now);
                }

                if (listConnect != nullptr)
                {
                    sLogInstance& inst{ listConnect[i] };
                    inst.liSource = static_cast<uint32_t>(instance.ciSource);
                    inst.liBitness = static_cast<uint32_t>(instance.ciBitness);
                    inst.liCookie = instance.ciCookie;
                    inst.liTimestamp = instance.ciTimestamp;
                    NEString::copyString( inst.liName
                                        , static_cast<NEString::CharCount>(LENGTH_NAME)
                                        , instance.ciInstance.getString()
                                        , static_cast<NEString::CharCount>(instance.ciInstance.getLength()));
                    NEString::copyString( inst.liLocation
                                        , static_cast<NEString::CharCount>(LENGTH_LOCATION)
                                        , instance.ciLocation.getString()
                                        , static_cast<NEString::CharCount>(instance.ciLocation.getLength()));
                }
            }

            if (listConnect == nullptr)
            {
                count = 0;
            }
        }
        else
        {
            evtInstDisconnected = mLoggerClient.mCallbacks->evtInstDisconnected;
            listDisconnect = count != 0 ? new ITEM_ID[count] : nullptr;
            if (listDisconnect != nullptr)
            {
                for (uint32_t i = 0; i < count; ++i)
                {
                    msgReceived >> listDisconnect[i];
                    const ITEM_ID& cookie{ listDisconnect[i] };

                    if (mLoggerClient.mInstances.removeAt(cookie))
                    {
                        mLoggerClient.mLogDatabase.logInstanceDisconnected(cookie, now);
                    }
                }
            }
            else
            {
                count = 0;
            }
        }

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (evtInstConnected != nullptr)
    {
        ASSERT(evtInstDisconnected == nullptr);
        ASSERT(listDisconnect == nullptr);
        evtInstConnected(static_cast<const sLogInstance *>(listConnect), count);
    }
    else if (evtInstDisconnected != nullptr)
    {
        ASSERT(evtInstConnected == nullptr);
        ASSERT(listConnect == nullptr);
        evtInstDisconnected(static_cast<const ITEM_ID *>(listDisconnect), count);
    }

    delete[] listDisconnect;
    delete[] listConnect;
}

void ObserverMessageProcessor::notifyLogRegisterScopes(const RemoteMessage& msgReceived)
{
    FuncLogRegisterScopes evtScopes{ nullptr };
    ITEM_ID cookie{ msgReceived.getSource() };
    sLogScope* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::getNow());

    do
    {
        Lock lock(mLoggerClient.mLock);
        mLoggerClient.mLogDatabase.logScopesDeactivate(cookie, now);
        evtScopes = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogRegisterScopes : nullptr;
        msgReceived >> count;
        scopes = count != 0 ? new sLogScope[count] : nullptr;
        if (scopes != nullptr)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                LogScope scope(msgReceived);
                sLogScope& entry{ scopes[i] };
                entry.lsId = scope.getScopeId();
                entry.lsPrio = scope.getPriority();
                NEString::copyString(entry.lsName, static_cast<NEString::CharCount>(LENGTH_SCOPE), scope.getScopeName().getString(), scope.getScopeName().getLength());

                mLoggerClient.mLogDatabase.logScopeActivate(entry.lsName, entry.lsId, entry.lsPrio, cookie, now);
            }
        }
        else
        {
            count = 0;
        }

        mLoggerClient.mLogDatabase.commit(true);

    } while (false);

    if (evtScopes != nullptr)
    {
        evtScopes(cookie, scopes, count);
    }

    delete[] scopes;
}

void ObserverMessageProcessor::notifyLogUpdateScopes(const RemoteMessage& msgReceived)
{
    FuncLogUpdateScopes evtScopes{ nullptr };
    ITEM_ID cookie{ msgReceived.getSource() };
    sLogScope* scopes{ nullptr };
    uint32_t count{ 0 };
    DateTime now(DateTime::getNow());

    do
    {
        Lock lock(mLoggerClient.mLock);
        mLoggerClient.mLogDatabase.logScopesDeactivate(cookie, now);
        evtScopes = mLoggerClient.mCallbacks != nullptr ? mLoggerClient.mCallbacks->evtLogUpdatedScopes : nullptr;
        msgReceived >> count;
        scopes = count != 0 ? new sLogScope[count] : nullptr;
        if (scopes != nullptr)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                LogScope scope(msgReceived);
                sLogScope& entry{ scopes[i] };
                entry.lsId = scope.getScopeId();
                entry.lsPrio = scope.getPriority();
                NEString::copyString(entry.lsName, static_cast<NEString::CharCount>(LENGTH_SCOPE), scope.getScopeName().getString(), scope.getScopeName().getLength());
                mLoggerClient.mLogDatabase.logScopeActivate(entry.lsName, entry.lsId, entry.lsPrio, cookie, now);
            }
        }
        else
        {
            count = 0;
        }

        mLoggerClient.mLogDatabase.commit(true);
    } while (false);

    if (evtScopes != nullptr)
    {
        evtScopes(cookie, scopes, count);
    }

    delete[] scopes;
}

void ObserverMessageProcessor::notifyLogMessage(const RemoteMessage& msgReceived)
{
    FuncLogMessage evtMessage{ nullptr };
    FuncLogMessageEx evtMessageEx{ nullptr };
    sLogMessage msgLog{ };
    const unsigned char* logBuffer{ nullptr };
    unsigned int size{ 0 };
    DateTime now{ DateTime::getNow() };

    do
    {
        Lock lock(mLoggerClient.mLock);
        const NELogging::sLogMessage* msgRemote = reinterpret_cast<const NELogging::sLogMessage*>(msgReceived.getBuffer());
        ASSERT(msgRemote != nullptr);
        mLoggerClient.mLogDatabase.logMessage(*msgRemote, now);
        mLoggerClient.mLogDatabase.commit(true);

        if (mLoggerClient.mCallbacks != nullptr)
        {
            if (mLoggerClient.mCallbacks->evtLogMessage != nullptr)
            {
                evtMessage = mLoggerClient.mCallbacks->evtLogMessage;

                msgLog.msgType      = static_cast<eLogType>(msgRemote->logMsgType);
                msgLog.msgPriority  = static_cast<eLogPriority>(msgRemote->logMessagePrio);
                msgLog.msgSource    = static_cast<unsigned long long>(msgRemote->logSource);
                msgLog.msgCookie    = static_cast<unsigned long long>(msgRemote->logCookie);
                msgLog.msgModuleId  = static_cast<unsigned long long>(msgRemote->logModuleId);
                msgLog.msgThreadId  = static_cast<unsigned long long>(msgRemote->logThreadId);
                msgLog.msgTimestamp = static_cast<unsigned long long>(msgRemote->logTimestamp);
                msgLog.msgScopeId   = static_cast<unsigned int>(msgRemote->logScopeId);

                NEString::copyString(msgLog.msgLogText, LENGTH_MESSAGE, msgRemote->logMessage, static_cast<NEString::CharCount>(msgRemote->logMessageLen));
                NEString::copyString(msgLog.msgThread, LENGTH_NAME, msgRemote->logThread     , static_cast<NEString::CharCount>(msgRemote->logThreadLen) );
                NEString::copyString(msgLog.msgModule, LENGTH_NAME, msgRemote->logModule     , static_cast<NEString::CharCount>(msgRemote->logModuleLen) );
            }
            else if (mLoggerClient.mCallbacks->evtLogMessageEx != nullptr)
            {
                evtMessageEx = mLoggerClient.mCallbacks->evtLogMessageEx;
                logBuffer = msgReceived.getBuffer();
                size = msgReceived.getSizeUsed();
            }
        }
    } while (false);

    if (evtMessage != nullptr)
    {
        evtMessage(&msgLog);
    }
    else if (evtMessageEx != nullptr)
    {
        evtMessageEx(logBuffer, size);
    }
}
