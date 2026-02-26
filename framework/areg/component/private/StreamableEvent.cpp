/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StreamableEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Streamable communication Event.
 *
 ************************************************************************/
#include "areg/component/StreamableEvent.hpp"

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // StreamableEvent class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Runtime implementation
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME_EVENT(StreamableEvent, areg::Event)

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    StreamableEvent::StreamableEvent( areg::Event::EventType eventType )
        : areg::Event (eventType)
    {
    }

    StreamableEvent::StreamableEvent( const areg::InStream & stream )
        : areg::Event ( areg::Event::EventType::EventUnknown )
    {
        stream >> areg::Event::mEventType;
    }

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    const areg::InStream & StreamableEvent::readStream( const areg::InStream & stream )
    {
        return (stream >> areg::Event::mEventType);
    }

    areg::OutStream & StreamableEvent::writeStream( areg::OutStream & stream ) const
    {
        return (stream << areg::Event::mEventType);
    }

} // namespace areg