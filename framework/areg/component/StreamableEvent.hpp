#ifndef AREG_COMPONENT_STREAMABLEEVENT_HPP
#define AREG_COMPONENT_STREAMABLEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/StreamableEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Streamable communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/Event.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// StreamableEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for events that serialize and deserialize data using streaming objects. Not
 *          created directly; use derived classes instead.
 **/
class AREG_API StreamableEvent : public Event
{
//////////////////////////////////////////////////////////////////////////
// friend classes
//////////////////////////////////////////////////////////////////////////
    friend class RemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Runtime declare
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes a streamable event with the specified event type.
     *
     * \param   eventType       The type of event.
     **/
    explicit StreamableEvent( Event::EventType eventType );

    /**
     * \brief   Initializes a streamable event by deserializing data from the given input stream.
     *
     * \param   stream      The input stream to read event data from.
     **/
    StreamableEvent( const InStream & stream );

    /**
     * \brief   Destructor
     **/
    virtual ~StreamableEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Deserializes event data from the given input stream.
     *
     * \param   stream      The input stream to read from.
     * \return  The input stream for method chaining.
     **/
    virtual const InStream & read_stream( const InStream & stream );

    /**
     * \brief   Serializes event data to the given output stream.
     *
     * \param   stream      The output stream to write to.
     * \return  The output stream for method chaining.
     **/
    virtual OutStream & write_stream( OutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StreamableEvent() = delete;
    AREG_NOCOPY_NOMOVE( StreamableEvent );
};

} // namespace areg
#endif  // AREG_COMPONENT_STREAMABLEEVENT_HPP
