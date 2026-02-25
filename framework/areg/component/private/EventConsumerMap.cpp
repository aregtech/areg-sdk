/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventConsumerMap.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Consumer Map class implementation
 *
 ************************************************************************/
#include "areg/component/private/EventConsumerMap.hpp"

#include "areg/component/EventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

EventConsumerList::~EventConsumerList()
{
    remove_all_consumers();
}

//////////////////////////////////////////////////////////////////////////
// EventConsumerList class, methods
//////////////////////////////////////////////////////////////////////////
bool EventConsumerList::add_consumer( EventConsumer& whichConsumer )
{
    bool result = false;
    if (EventConsumerListBase::push_last_unique(&whichConsumer))
    {
        result = true;
        whichConsumer.consumer_registered(true);
    }
    
    return result;
}

bool EventConsumerList::remove_consumer( EventConsumer& whichConsumer )
{
    bool result = false;
    if ( EventConsumerListBase::remove_entry(&whichConsumer) )
    {
        result = true;
        whichConsumer.consumer_registered(false);
    }

    return result;
}

void EventConsumerList::remove_all_consumers()
{
    EventConsumerListBase::LISTPOS pos = EventConsumerListBase::first_position();
    for (; is_valid_position(pos); pos = next_position(pos))
    {
        EventConsumer* consumer = value_at_position(pos);
        ASSERT(consumer != nullptr);
        consumer->consumer_registered(false);
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

void ImplEventConsumerMap::impl_clean_resource( RuntimeClassID & Key, EventConsumerList * Resource )
{
    AREG_OUTPUT_DBG("Resource [ %s ]: Removing all consumers and deleting resource at address [ %p ]", Key.name().as_string(), Resource);
    ASSERT(Resource != nullptr);
    Resource->remove_all_consumers();
    delete Resource;
    Resource = nullptr;
}

#else   // !(defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG))

void ImplEventConsumerMap::impl_clean_resource( RuntimeClassID & /*Key*/, EventConsumerList * Resource )
{
    Resource->remove_all_consumers();
    delete Resource;
    Resource = nullptr;
}

#endif  // defined(DEBUG) && defined(OUTPUT_DEBUG_LEVEL) && (OUTPUT_DEBUG_LEVEL >= OUTPUT_DEBUG_LEVEL_DEBUG)
