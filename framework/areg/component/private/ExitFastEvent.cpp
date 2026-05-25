/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ExitFastEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, fast exit event implementation.
 *
 ************************************************************************/
#include "areg/component/ExitFastEvent.hpp"

namespace areg {

AREG_IMPLEMENT_RUNTIME_EVENT(ExitFastEvent, Event)

ExitFastEvent::ExitFastEvent()
    : Event ( areg::EventType::EventExternal )
{
    mEventPrio = areg::EventPriority::ExitPrio;
}

ExitFastEvent & ExitFastEvent::exit_event() noexcept
{
    static ExitFastEvent _exitFastEvent;
    return _exitFastEvent;
}

void ExitFastEvent::destroy()
{
}

} // namespace areg
