/************************************************************************
 * \file        areg/component/private/CEExitEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Exit Event declaration.
 *              All dispatchers receiving this event should be stopped
 *              and start preparing exit procedure
 *
 ************************************************************************/
#include "areg/component/private/CEExitEvent.hpp"

IMPLEMENT_RUNTIME_EVENT(CEExitEvent, CEEvent)

CEExitEvent::CEExitEvent( void )
    : CEEvent   ( CEEvent::EventExternal )
{
    ; // do nothing
}

CEExitEvent::~CEExitEvent( void )
{
    ; // do nothing
}

CEExitEvent & CEExitEvent::GetExitEvent( void )
{
    static CEExitEvent  _exitEvent;
    return _exitEvent;
}

void CEExitEvent::Destroy( void )
{
    ; // do nothing
}
