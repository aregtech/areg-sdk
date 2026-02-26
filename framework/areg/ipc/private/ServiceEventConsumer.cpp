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

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // ReconnectTimerConsumer class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::ReconnectTimerConsumer::ReconnectTimerConsumer(ServiceEventConsumer& eventConsumer )
        : areg::TimerConsumer       ( )
        , mServiceEventConsumer ( eventConsumer )
    {
    }

    void areg::ReconnectTimerConsumer::processTimer(areg::Timer& /* timer */)
    {
        mServiceEventConsumer.onServiceReconnectTimerExpired();
    }

    //////////////////////////////////////////////////////////////////////////
    // ServiceClientConsumer class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::ServiceClientConsumer::ServiceClientConsumer(ServiceEventConsumer& eventConsumer)
        : areg::ServiceClientEventConsumer( )
        , mServiceEventConsumer     ( eventConsumer )
    {
    }

    void areg::ServiceClientConsumer::processEvent(const areg::ServiceEventData& data)
    {
        switch (data.getCommand())
        {
        case areg::ServiceEventData::ServiceCommand::CMD_StartService:
            mServiceEventConsumer.onServiceStart();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_StopService:
            mServiceEventConsumer.onServiceStop();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_RestartService:
            mServiceEventConsumer.onServiceRestart();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceStarted:
            mServiceEventConsumer.onServiceConnectionStarted();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceStopped:
            mServiceEventConsumer.onServiceConnectionStopped();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceLost:
            mServiceEventConsumer.onServiceConnectionLost();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceExit:
            mServiceEventConsumer.onServiceExit();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg:
            mServiceEventConsumer.onServiceMessageReceived(data.getMessage());
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceSendMsg:
            mServiceEventConsumer.onServiceMessageSend(data.getMessage());
            break;

        default:
            ASSERT(false);
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // ServiceServerConsumer class implementation
    //////////////////////////////////////////////////////////////////////////

    areg::ServiceServerConsumer::ServiceServerConsumer(ServiceEventConsumer& serviceEventConsumer)
        : areg::ServiceServerEventConsumer( )
        , mServiceEventConsumer     ( serviceEventConsumer )
    {
    }

    void areg::ServiceServerConsumer::processEvent(const areg::ServiceEventData& data)
    {
        switch (data.getCommand())
        {
        case areg::ServiceEventData::ServiceCommand::CMD_StartService:
            mServiceEventConsumer.onServiceStart();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_StopService:
            mServiceEventConsumer.onServiceStop();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_RestartService:
            mServiceEventConsumer.onServiceRestart();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceStarted:
            mServiceEventConsumer.onServiceConnectionStarted();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceStopped:
            mServiceEventConsumer.onServiceConnectionStopped();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceLost:
            mServiceEventConsumer.onServiceConnectionLost();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceExit:
            mServiceEventConsumer.onServiceExit();
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceReceivedMsg:
            mServiceEventConsumer.onServiceMessageReceived(data.getMessage());
            break;

        case areg::ServiceEventData::ServiceCommand::CMD_ServiceSendMsg:
            mServiceEventConsumer.onServiceMessageSend(data.getMessage());
            break;

        default:
            ASSERT(false);
            break;
        }
    }

} // namespace areg