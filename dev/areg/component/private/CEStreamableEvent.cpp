/************************************************************************
 * \file        areg/component/private/CEStreamableEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Streamable communication Event.
 *
 ************************************************************************/
#include "areg/component/CEStreamableEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CEStreamableEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEStreamableEvent, CEThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEStreamableEvent::CEStreamableEvent( CEEvent::eEventType eventType )
    : CEThreadEventBase (eventType)
{
    ; // do nothing
}

CEStreamableEvent::CEStreamableEvent( const IEInStream & stream )
    : CEThreadEventBase ( CEEvent::EventUnknown )
{
    stream >> CEEvent::mEventType;
}

CEStreamableEvent::~CEStreamableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
const IEInStream & CEStreamableEvent::ReadFromStream( const IEInStream & stream )
{
    return (stream >> CEEvent::mEventType);
}

IEOutStream & CEStreamableEvent::WriteToStream( IEOutStream & stream ) const
{
    return (stream << CEEvent::mEventType);
}
