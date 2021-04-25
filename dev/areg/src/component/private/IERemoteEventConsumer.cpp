/************************************************************************
 * \file        areg/src/component/private/IERemoteEventConsumer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Remote Event consumer to dispatch remote events.
 *
 ************************************************************************/
#include "areg/src/component/IERemoteEventConsumer.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"

IERemoteEventConsumer::IERemoteEventConsumer(void)
{

}

IERemoteEventConsumer::~IERemoteEventConsumer(void)
{

}

void IERemoteEventConsumer::StartEventProcessing(CEEvent & eventElem)
{
    if ( (eventElem.GetEventType() & CEEvent::EventRemote) != 0 )
    {
        CERemoteRequestEvent * requestEvent = RUNTIME_CAST(&eventElem, CERemoteRequestEvent);
        if ( requestEvent != NULL )
        {
            ProcessRemoteEvent(*requestEvent);
        }
        else
        {
            CERemoteResponseEvent * responseEvent = RUNTIME_CAST(&eventElem, CERemoteResponseEvent);
            if ( responseEvent != NULL )
            {
                ProcessRemoteEvent(*responseEvent);
            }
            else
            {
                CERemoteNotifyRequestEvent * requestNotifyEvent = RUNTIME_CAST(&eventElem, CERemoteNotifyRequestEvent);
                if ( requestNotifyEvent != NULL )
                    ProcessRemoteEvent(*requestNotifyEvent);
            }
        }
    }
}
