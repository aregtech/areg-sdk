/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ServiceEventConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Event consumers
 ************************************************************************/

#include "areg/ipc/ServiceEventConsumer.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ReconnectTimerConsumer::ReconnectTimerConsumer(ServiceEventConsumer& eventConsumer )
    : TimerConsumer       ( )
    , mServiceEventConsumer ( eventConsumer )
{
}

void ReconnectTimerConsumer::process_timer(Timer& /* timer */)
{
    mServiceEventConsumer.on_reconnect_timer();
}

//////////////////////////////////////////////////////////////////////////
// ServiceClientConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceClientConsumer::ServiceClientConsumer(ServiceEventConsumer& eventConsumer)
    : ServiceClientEventConsumer( )
    , mServiceEventConsumer     ( eventConsumer )
{
}

void ServiceClientConsumer::process_event(const ServiceEventData& data)
{
    switch (data.command())
    {
    case ServiceEventData::ServiceCommand::CMD_StartService:
        mServiceEventConsumer.on_service_start();
        break;

    case ServiceEventData::ServiceCommand::CMD_StopService:
        mServiceEventConsumer.on_service_stop();
        break;

    case ServiceEventData::ServiceCommand::CMD_RestartService:
        mServiceEventConsumer.on_service_restart();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceStarted:
        mServiceEventConsumer.on_connection_started();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceStopped:
        mServiceEventConsumer.on_connection_stopped();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceLost:
        mServiceEventConsumer.on_connection_lost();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceExit:
        mServiceEventConsumer.on_service_exit();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg:
        mServiceEventConsumer.on_message_received(data.message());
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceSendMsg:
        mServiceEventConsumer.on_message_send(data.message());
        break;

    default:
        ASSERT(false);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
// ServiceServerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

ServiceServerConsumer::ServiceServerConsumer(ServiceEventConsumer& serviceEventConsumer)
    : ServiceServerEventConsumer( )
    , mServiceEventConsumer     ( serviceEventConsumer )
{
}

void ServiceServerConsumer::process_event(const ServiceEventData& data)
{
    switch (data.command())
    {
    case ServiceEventData::ServiceCommand::CMD_StartService:
        mServiceEventConsumer.on_service_start();
        break;

    case ServiceEventData::ServiceCommand::CMD_StopService:
        mServiceEventConsumer.on_service_stop();
        break;

    case ServiceEventData::ServiceCommand::CMD_RestartService:
        mServiceEventConsumer.on_service_restart();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceStarted:
        mServiceEventConsumer.on_connection_started();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceStopped:
        mServiceEventConsumer.on_connection_stopped();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceLost:
        mServiceEventConsumer.on_connection_lost();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceExit:
        mServiceEventConsumer.on_service_exit();
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg:
        mServiceEventConsumer.on_message_received(data.message());
        break;

    case ServiceEventData::ServiceCommand::CMD_ServiceSendMsg:
        mServiceEventConsumer.on_message_send(data.message());
        break;

    default:
        ASSERT(false);
        break;
    }
}

} // namespace areg
