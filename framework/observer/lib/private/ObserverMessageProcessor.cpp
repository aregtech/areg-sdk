/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/private/ObserverMessageProcessor.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The log observer message processor.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "observer/lib/private/ObserverMessageProcessor.hpp"

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/TEArrayList.hpp"
#include "areg/component/NEService.hpp"
#include "areg/trace/TraceScope.hpp"

#include "observer/lib/private/LoggerClient.hpp"
#include "observer/lib/LogObserverApi.h"

#define IS_VALID(callback)  ((mLoggerClient.mCallbacks != nullptr) && ((callback) != nullptr))


ObserverMessageProcessor::ObserverMessageProcessor(LoggerClient& loggerClient)
    : mLoggerClient (loggerClient)
{
}

void ObserverMessageProcessor::notifyServiceConnection(const RemoteMessage& msgReceived)
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
                ASSERT(cookie == msgReceived.getTarget());
                mLoggerClient.mClientConnection.setCookie(cookie);
                mLoggerClient.sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStarted);
            }
            else
            {
                mLoggerClient.cancelConnection();
                mLoggerClient.sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
            }
        }
        break;

    case NEService::eServiceConnection::ServiceRejected:
        {
            mLoggerClient.cancelConnection();
            mLoggerClient.sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceStopped);
        }
        break;

    default:
        {
            mLoggerClient.cancelConnection();
            mLoggerClient.sendCommand(ServiceEventData::eServiceEventCommands::CMD_ServiceLost);
        }
        break;
    }
}

void ObserverMessageProcessor::notifyConnectedClients(const RemoteMessage& msgReceived)
{
    if (IS_VALID(mLoggerClient.mCallbacks->evtLogSources))
    {
        uint32_t count{ 0 };
        msgReceived >> count;

        sLogInstance* list = new sLogInstance[count];
        if (list != nullptr)
        {
            for (uint32_t i = 0; i < count; ++i)
            {
                NEService::sServiceConnectedInstance instance{};
                msgReceived >> instance;
                list[i].liSource    = static_cast<uint32_t>(instance.ciSource);
                list[i].liBitness   = static_cast<uint32_t>(instance.ciBitness);
                list[i].liCookie    = instance.ciCookie;
                NEString::copyString( list[i].liName
                                    , static_cast<NEString::CharCount>(LENGTH_NAME)
                                    , instance.ciInstance.getString()
                                    , static_cast<NEString::CharCount>(instance.ciInstance.getLength()));
                NEString::copyString( list[i].liLocation
                                    , static_cast<NEString::CharCount>(LENGTH_LOCATION)
                                    , instance.ciLocation.getString()
                                    , static_cast<NEString::CharCount>(instance.ciLocation.getLength()));
            }
        }

        mLoggerClient.mCallbacks->evtLogSources(list, list != nullptr ? count : 0);
        delete[] list;
    }
}

void ObserverMessageProcessor::notifyLogRegisterScopes(const RemoteMessage& msgReceived)
{
    if (IS_VALID(mLoggerClient.mCallbacks->evtLogScopes))
    {
        ITEM_ID cookie{ msgReceived.getSource() };
        uint32_t count{ 0 };
        msgReceived >> count;
        sLogScope* scopes = new sLogScope[count];
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

        mLoggerClient.mCallbacks->evtLogScopes(cookie, scopes, scopes != nullptr ? count : 0u);
        delete[] scopes;
    }
}

void ObserverMessageProcessor::notifyLogMessage(const RemoteMessage& msgReceived)
{
    if (IS_VALID(mLoggerClient.mCallbacks->evtLogMessage))
    {
        const NETrace::sLogMessage * msgRemote = reinterpret_cast<const NETrace::sLogMessage *>(msgReceived.getBuffer());
        ASSERT(msgRemote != nullptr);

        sLogMessage msgLog{ };
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

        mLoggerClient.mCallbacks->evtLogMessage(&msgLog);
    }
}
