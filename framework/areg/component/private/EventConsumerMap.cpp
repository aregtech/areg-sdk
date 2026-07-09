/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/EventConsumerMap.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Consumer Map class implementation
 *
 ************************************************************************/
#include "areg/component/private/EventConsumerMap.hpp"

#include "areg/component/EventConsumer.hpp"

namespace areg {

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
    if (!EventConsumerListBase::add_if_unique(&whichConsumer))
        return false;
    
    whichConsumer.consumer_registered(true);
    return true;
}

bool EventConsumerList::remove_consumer( EventConsumer& whichConsumer ) noexcept
{
    if (!EventConsumerListBase::remove_elem(&whichConsumer))
        return false;
    
    whichConsumer.consumer_registered(false);
    return true;
}

void EventConsumerList::remove_all_consumers() noexcept
{
    for (auto consumer : EventConsumerList::data())
    {
        ASSERT(consumer != nullptr);
        consumer->consumer_registered(false);
    }

    EventConsumerListBase::clear();
}

} // namespace areg
