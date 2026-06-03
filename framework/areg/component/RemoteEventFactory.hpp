#ifndef AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
#define AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote Event Factory class. 
 *              Creates event from stream and converts event to stream.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class Event;
class EventEnvelope;
class RemoteResponseEvent;
class Channel;

//////////////////////////////////////////////////////////////////////////
// RemoteEventFactory class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Factory for creating remote event objects from streams or serializing events to streams
 *          for inter-process communication.
 **/
class AREG_API RemoteEventFactory
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates an Event from a received wire envelope. Returns an invalid Event on
     *          unrecognised or unsupported event types.
     *
     * \param   stream          The received wire envelope.
     * \param   comChannel      The communication channel for routing correction.
     * \return  An Event value; check is_valid() before delivering.
     **/
    [[nodiscard]]
    static Event event_from_stream( const EventEnvelope & stream, const Channel & comChannel );

    /**
     * \brief   Serializes a remote event into a wire-ready EventEnvelope with routing fields set.
     *          The event must be a remote type; local and custom types are rejected (ASSERT).
     *
     * \param   stream          Output envelope to fill.
     * \param   eventElem       Source event (must be remote type).
     * \param   comChannel      Communication channel providing source/target cookies.
     * \return  Returns true if the event was recognised and serialized; false otherwise.
     **/
    static bool stream_from_event( EventEnvelope & stream, const Event & eventElem, const Channel & comChannel );

    /**
     * \brief   Creates a request-failure response Event from an undeliverable wire envelope.
     *          Returns an invalid Event when the proxy cannot be located.
     *
     * \param   stream          The original wire envelope that could not be processed.
     * \param   comChannel      The communication channel for routing correction.
     * \return  An Event value; check is_valid() before delivering.
     **/
    [[nodiscard]]
    static Event request_failed_event( const EventEnvelope & stream, const Channel & comChannel );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Hidden
//////////////////////////////////////////////////////////////////////////
private:
    RemoteEventFactory() = default;

    ~RemoteEventFactory() = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( RemoteEventFactory );
};

} // namespace areg
#endif  // AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
