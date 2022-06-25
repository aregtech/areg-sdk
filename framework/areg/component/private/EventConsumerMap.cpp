/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventConsumerMap.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Consumer Map class implementation
 *
 ************************************************************************/
#include "areg/component/private/EventConsumerMap.hpp"

#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

EventConsumerList::~EventConsumerList( void )
{
    removeAllConsumers();
}

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, methods
//////////////////////////////////////////////////////////////////////////
bool EventConsumerList::addConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if (EventConsumerListBase::pushLastIfNew(&whichConsumer))
    {
        result = true;
        whichConsumer.consumerRegistered(true);
    }
    
    return result;
}

bool EventConsumerList::removeConsumer( IEEventConsumer& whichConsumer )
{
    bool result = false;
    if ( EventConsumerListBase::removeEntry(&whichConsumer) )
    {
        result = true;
        whichConsumer.consumerRegistered(false);
    }

    return result;
}

void EventConsumerList::removeAllConsumers( void )
{
    EventConsumerListBase::LISTPOS pos = EventConsumerListBase::firstPosition();
    for (; isValidPosition(pos); pos = nextPosition(pos))
    {
        IEEventConsumer* consumer = valueAtPosition(pos);
        ASSERT(consumer != nullptr);
        consumer->consumerRegistered(false);
    }

    EventConsumerListBase::clear();
}

//////////////////////////////////////////////////////////////////////////
// ImplEventConsumerMap class implementation
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ImplEventConsumerMap class, methods
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
void ImplEventConsumerMap::implCleanResource( RuntimeClassID & Key, EventConsumerList * Resource )
{
    OUTPUT_DBG("Resource [ %s ]: Removing all consumers and deleting resource at address [ %p ]", Key.getName(), Resource);
    ASSERT(Resource != nullptr);
    Resource->removeAllConsumers();
    delete Resource;
}

#else
void ImplEventConsumerMap::implCleanResource( RuntimeClassID & /*Key*/, EventConsumerList * Resource )
{
    Resource->removeAllConsumers();
    delete Resource;
}
#endif
