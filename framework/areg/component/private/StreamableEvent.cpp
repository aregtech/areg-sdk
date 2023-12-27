/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StreamableEvent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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
}

StreamableEvent::StreamableEvent( const IEInStream & stream )
    : ThreadEventBase ( Event::eEventType::EventUnknown )
{
    stream >> Event::mEventType;
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
