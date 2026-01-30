/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceEventConsumerBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Event consumers
 ************************************************************************/

#include "areg/ipc/ServiceEventConsumerBase.hpp"

//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ReconnectTimerConsumer::ReconnectTimerConsumer(IEServiceEventConsumerBase& eventConsumer )
    : IETimerConsumer       ( )
    , mServiceEventConsumer ( eventConsumer )
{
}

void ReconnectTimerConsumer::processTimer(Timer& /* timer */)
{
    mServiceEventConsumer.onServiceReconnectTimerExpired();
}

//////////////////////////////////////////////////////////////////////////
// ServiceClientEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceClientEventConsumer::ServiceClientEventConsumer(IEServiceEventConsumerBase& eventConsumer)
    : IEServiceClientEventConsumer  ( )
    , mServiceEventConsumer         ( eventConsumer )
{
}

void ServiceClientEventConsumer::processEvent(const ServiceEventData& data)
{
    switch (data.getCommand())
    {
    case ServiceEventData::eServiceEventCommands::CMD_StartService:
        mServiceEventConsumer.onServiceStart();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_StopService:
        mServiceEventConsumer.onServiceStop();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_RestartService:
        mServiceEventConsumer.onServiceRestart();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceStarted:
        mServiceEventConsumer.onServiceConnectionStarted();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceStopped:
        mServiceEventConsumer.onServiceConnectionStopped();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceLost:
        mServiceEventConsumer.onServiceConnectionLost();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceExit:
        mServiceEventConsumer.onServiceExit();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg:
        mServiceEventConsumer.onServiceMessageReceived(data.getMessage());
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceSendMsg:
        mServiceEventConsumer.onServiceMessageSend(data.getMessage());
        break;

    default:
        ASSERT(false);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// ServiceServerEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceServerEventConsumer::ServiceServerEventConsumer(IEServiceEventConsumerBase& serviceEventConsumer)
    : IEServiceServerEventConsumer  ( )
    , mServiceEventConsumer         ( serviceEventConsumer )
{
}

void ServiceServerEventConsumer::processEvent(const ServiceEventData& data)
{
    switch (data.getCommand())
    {
    case ServiceEventData::eServiceEventCommands::CMD_StartService:
        mServiceEventConsumer.onServiceStart();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_StopService:
        mServiceEventConsumer.onServiceStop();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_RestartService:
        mServiceEventConsumer.onServiceRestart();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceStarted:
        mServiceEventConsumer.onServiceConnectionStarted();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceStopped:
        mServiceEventConsumer.onServiceConnectionStopped();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceLost:
        mServiceEventConsumer.onServiceConnectionLost();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceExit:
        mServiceEventConsumer.onServiceExit();
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg:
        mServiceEventConsumer.onServiceMessageReceived(data.getMessage());
        break;

    case ServiceEventData::eServiceEventCommands::CMD_ServiceSendMsg:
        mServiceEventConsumer.onServiceMessageSend(data.getMessage());
        break;

    default:
        ASSERT(false);
        break;
    }
}
