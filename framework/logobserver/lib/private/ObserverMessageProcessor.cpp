/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logobserver/lib/private/ObserverMessageProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/lib/private/ObserverMessageProcessor.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/component/NEService.hpp"
#include "areg/ipc/NERemoteService.hpp"
#include "areg/trace/TraceScope.hpp"

#include "logobserver/lib/LogObserverApi.h"
#include "logobserver/lib/private/LoggerClient.hpp"

#define IS_VALID(callback)  ((mLoggerClient.mCallbacks != nullptr) && ((callback) != nullptr))


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

    do
    {
        Lock lock(mLoggerClient.mLock);
        if (mLoggerClient.mCallbacks != nullptr)
        {
            msgReceived >> remConnect;
            msgReceived >> count;

            if (remConnect == NERemoteService::eRemoteConnection::RemoteConnected)
            {
                evtInstConnected = mLoggerClient.mCallbacks->evtInstConnected;
                listConnect = count != 0 ? new sLogInstance[count] : nullptr;
                if (listConnect != nullptr)
                {
                    for (uint32_t i = 0; i < count; ++i)
                    {
                        sLogInstance& inst{ listConnect[i] };
                        NEService::sServiceConnectedInstance instance{};
                        msgReceived >> instance;
                        inst.liSource   = static_cast<uint32_t>(instance.ciSource);
                        inst.liBitness  = static_cast<uint32_t>(instance.ciBitness);
                        inst.liCookie   = instance.ciCookie;
                        inst.liTimestamp= instance.ciTimestamp;
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
                else
                {
                    count = 0;
                }
            }
            else
            {
                evtInstDisconnected = mLoggerClient.mCallbacks->evtInstDisconnected;
                listDisconnect = new ITEM_ID[count];
                if (listDisconnect != nullptr)
                {
                    for (uint32_t i = 0; i < count; ++i)
                    {
                        msgReceived >> listDisconnect[i];                        
                    }
                }
                else
                {
                    count = 0;
                }
            }
        }
    } while (false);

    if (evtInstConnected != nullptr)
    {
        ASSERT(evtInstDisconnected == nullptr);
        ASSERT(listDisconnect == nullptr);
        evtInstConnected(static_cast<const sLogInstance *>(listConnect), count);
        if (listConnect != nullptr)
        {
            delete[] listConnect;
        }
    }
    else if (evtInstDisconnected != nullptr)
    {
        ASSERT(evtInstConnected == nullptr);
        ASSERT(listConnect == nullptr);
        evtInstDisconnected(static_cast<const ITEM_ID *>(listDisconnect), count);
        if (listDisconnect != nullptr)
        {
            delete[] listDisconnect;
        }
    }
}

void ObserverMessageProcessor::notifyLogRegisterScopes(const RemoteMessage& msgReceived)
{
    FuncLogScopes evtScopes{ nullptr };
    ITEM_ID cookie{ msgReceived.getSource() };
    sLogScope* scopes{ nullptr };
    uint32_t count{ 0 };

    do
    {
        Lock lock(mLoggerClient.mLock);
        if (mLoggerClient.mCallbacks != nullptr)
        {
            evtScopes = mLoggerClient.mCallbacks->evtLogScopes;
            msgReceived >> count;
            scopes = count != 0 ? new sLogScope[count] : nullptr;
            if (scopes != nullptr)
            {
                for (uint32_t i = 0; i < count; ++i)
                {
                    TraceScope scope(msgReceived);
                    scopes[i].lsId = scope.getScopeId();
                    scopes[i].lsPrio = scope.getPriority();
                    NEString::copyString(scopes[i].lsName, static_cast<NEString::CharCount>(LENGTH_SCOPE), scope.getScopeName().getString(), scope.getScopeName().getLength());
                }
            }
            else
            {
                count = 0;
            }
        }
    } while (false);

    if (evtScopes != nullptr)
    {
        evtScopes(cookie, scopes, count);
        if (scopes != nullptr)
        {
            delete[] scopes;
        }
    }
}

void ObserverMessageProcessor::notifyLogMessage(const RemoteMessage& msgReceived)
{
    FuncLogMessage evtMessage{ nullptr };
    FuncLogMessageEx evtMessageEx{ nullptr };
    sLogMessage msgLog{ };
    const unsigned char* logBuffer{ nullptr };
    unsigned int size{ 0 };

    do
    {
        Lock lock(mLoggerClient.mLock);
        if (mLoggerClient.mCallbacks != nullptr)
        {
            if (mLoggerClient.mCallbacks->evtLogMessage != nullptr)
            {
                evtMessage = mLoggerClient.mCallbacks->evtLogMessage;
                const NETrace::sLogMessage* msgRemote = reinterpret_cast<const NETrace::sLogMessage*>(msgReceived.getBuffer());
                ASSERT(msgRemote != nullptr);

                msgLog.msgType      = static_cast<eLogType>(msgRemote->logMsgType);
                msgLog.msgPriority  = static_cast<eLogPriority>(msgRemote->logMessagePrio);
                msgLog.msgSource    = static_cast<unsigned long long>(msgRemote->logSource);
                msgLog.msgCookie    = static_cast<unsigned long long>(msgRemote->logCookie);
                msgLog.msgModuleId  = static_cast<unsigned long long>(msgRemote->logModuleId);
                msgLog.msgThreadId  = static_cast<unsigned long long>(msgRemote->logThreadId);
                msgLog.msgTimestamp = static_cast<unsigned long long>(msgRemote->logTimestamp);
                msgLog.msgScopeId   = static_cast<unsigned int>(msgRemote->logScopeId);

                NEString::copyString(msgLog.msgLogText, LENGTH_MESSAGE, msgRemote->logMessage, msgRemote->logMessageLen);
                NEString::copyString(msgLog.msgThread, LENGTH_NAME, msgRemote->logThread, msgRemote->logThreadLen);
                NEString::copyString(msgLog.msgModule, LENGTH_NAME, msgRemote->logModule, msgRemote->logModuleLen);
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
