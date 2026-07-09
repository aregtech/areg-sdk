/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/ExitEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/component/ExitEvent.hpp"
#include "areg/base/RuntimeClassID.hpp"

namespace areg {


AREG_IMPLEMENT_RUNTIME_EVENT(ExitEvent, Event)

ExitEvent::ExitEvent()
    : Event   ( areg::EventType::EventCustomExternal, areg::EventPriority::ExitPrio )
{
    set_event_id(ExitEvent::CLASS_ID);
}

const ExitEvent & ExitEvent::exit_event() noexcept
{
    static ExitEvent  _exitEvent;
    return _exitEvent;
}

} // namespace areg
