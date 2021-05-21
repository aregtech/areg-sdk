/************************************************************************
 * \file        areg/component/private/IERemoteEventConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
#include "areg/component/IERemoteEventConsumer.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"

IERemoteEventConsumer::IERemoteEventConsumer(void)
{

}

IERemoteEventConsumer::~IERemoteEventConsumer(void)
{

}

void IERemoteEventConsumer::startEventProcessing(Event & eventElem)
{
    if ( (eventElem.getEventType() & Event::EventRemote) != 0 )
    {
        RemoteRequestEvent * requestEvent = RUNTIME_CAST(&eventElem, RemoteRequestEvent);
        if ( requestEvent != NULL )
        {
            processRemoteRequestEvent(*requestEvent);
        }
        else
        {
            RemoteResponseEvent * responseEvent = RUNTIME_CAST(&eventElem, RemoteResponseEvent);
            if ( responseEvent != NULL )
            {
                processRemoteResponseEvent(*responseEvent);
            }
            else
            {
                RemoteNotifyRequestEvent * requestNotifyEvent = RUNTIME_CAST(&eventElem, RemoteNotifyRequestEvent);
                if ( requestNotifyEvent != NULL )
                    processRemoteNotifyRequestEvent(*requestNotifyEvent);
            }
        }
    }
}
