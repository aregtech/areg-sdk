/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/RemoteEventConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
#include "areg/component/RemoteEventConsumer.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"

namespace areg
{
    void RemoteEventConsumer::startEventProcessing(areg::Event & eventElem)
    {
        if ( areg::Event::isRemote(eventElem.getEventType()) )
        {
            areg::RemoteRequestEvent * requestEvent = AREG_RUNTIME_CAST(&eventElem, areg::RemoteRequestEvent);
            if ( requestEvent != nullptr )
            {
                processRemoteRequestEvent(*requestEvent);
            }
            else
            {
                areg::RemoteResponseEvent * responseEvent = AREG_RUNTIME_CAST(&eventElem, areg::RemoteResponseEvent);
                if ( responseEvent != nullptr )
                {
                    processRemoteResponseEvent(*responseEvent);
                }
                else
                {
                    areg::RemoteNotifyRequestEvent * requestNotifyEvent = AREG_RUNTIME_CAST(&eventElem, areg::RemoteNotifyRequestEvent);
                    if (requestNotifyEvent != nullptr)
                    {
                        processRemoteNotifyRequestEvent(*requestNotifyEvent);
                    }
                }
            }
        }
    }

} // namespace areg