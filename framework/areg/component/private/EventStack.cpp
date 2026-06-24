/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventStack.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, non-concurrent FIFO event queue implementation.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/component/private/EventStack.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/RuntimeClassID.hpp"
#include "areg/component/Event.hpp"
#include "areg/logging/areg_log.h"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventStack -- constructor / destructor
//////////////////////////////////////////////////////////////////////////

EventStack::EventStack() noexcept
    : Stack<Event>   ( )
{
}

EventStack::~EventStack() noexcept
{
    mValueList.clear();
}

//////////////////////////////////////////////////////////////////////////
// EventStack -- bulk-removal operations
//////////////////////////////////////////////////////////////////////////

void EventStack::remove_all_events() noexcept
{
    mValueList.clear();
}

uint32_t EventStack::delete_matching(uint32_t eventClassId) noexcept
{
    auto it = mValueList.begin();
    while (it != mValueList.end())
    {
        if ((it->event_priority() != areg::EventPriority::ExitPrio) && (eventClassId == it->event_id()))
            it = mValueList.erase(it);
        else
            ++it;
    }

    return static_cast<uint32_t>(mValueList.size());
}

uint32_t EventStack::delete_except(uint32_t eventClassId) noexcept
{
    auto it = mValueList.begin();
    while (it != mValueList.end())
    {
        if ((it->event_priority() != areg::EventPriority::ExitPrio) && (eventClassId != it->event_id()))
            it = mValueList.erase(it);
        else
            ++it;
    }

    return static_cast<uint32_t>(mValueList.size());
}

} // namespace areg
