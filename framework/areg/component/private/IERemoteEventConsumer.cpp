/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/IERemoteEventConsumer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
#include "areg/component/IERemoteEventConsumer.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"

void IERemoteEventConsumer::startEventProcessing(Event & eventElem)
{
    if ( Event::isRemote(eventElem.getEventType()) )
    {
        RemoteRequestEvent * requestEvent = RUNTIME_CAST(&eventElem, RemoteRequestEvent);
        if ( requestEvent != nullptr )
        {
            processRemoteRequestEvent(*requestEvent);
        }
        else
        {
            RemoteResponseEvent * responseEvent = RUNTIME_CAST(&eventElem, RemoteResponseEvent);
            if ( responseEvent != nullptr )
            {
                processRemoteResponseEvent(*responseEvent);
            }
            else
            {
                RemoteNotifyRequestEvent * requestNotifyEvent = RUNTIME_CAST(&eventElem, RemoteNotifyRequestEvent);
                if (requestNotifyEvent != nullptr)
                {
                    processRemoteNotifyRequestEvent(*requestNotifyEvent);
                }
            }
        }
    }
}
