/************************************************************************
 * \file        areg/component/private/StreamableEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Streamable communication Event.
 *
 ************************************************************************/
#include "areg/component/StreamableEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// StreamableEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(StreamableEvent, ThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
StreamableEvent::StreamableEvent( Event::eEventType eventType )
    : ThreadEventBase (eventType)
{
    ; // do nothing
}

StreamableEvent::StreamableEvent( const IEInStream & stream )
    : ThreadEventBase ( Event::EventUnknown )
{
    stream >> Event::mEventType;
}

StreamableEvent::~StreamableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
const IEInStream & StreamableEvent::readStream( const IEInStream & stream )
{
    return (stream >> Event::mEventType);
}

IEOutStream & StreamableEvent::writeStream( IEOutStream & stream ) const
{
    return (stream << Event::mEventType);
}
