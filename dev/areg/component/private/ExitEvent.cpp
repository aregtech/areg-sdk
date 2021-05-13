/************************************************************************
 * \file        areg/component/private/ExitEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/component/private/ExitEvent.hpp"

IMPLEMENT_RUNTIME_EVENT(ExitEvent, Event)

ExitEvent::ExitEvent( void )
    : Event   ( Event::EventExternal )
{
    ; // do nothing
}

ExitEvent::~ExitEvent( void )
{
    ; // do nothing
}

ExitEvent & ExitEvent::getExitEvent( void )
{
    static ExitEvent  _exitEvent;
    return _exitEvent;
}

void ExitEvent::destroy( void )
{
    ; // do nothing
}
