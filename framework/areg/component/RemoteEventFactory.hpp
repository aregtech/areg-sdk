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
class RemoteResponseEvent;
class StreamableEvent;
class RemoteMessage;
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
     * \brief   Creates event from streaming object. Returns event object of appropriate type.
     *          Automatically deleted by dispatcher after processing.
     *
     * \param   stream          The streaming object containing event data.
     * \param   comChannel      The communication channel object to send event.
     * \return  Returns valid pointer to remote event object if successful, or nullptr if streaming
     *          data contains wrong or unsupported event types.
     **/
    [[nodiscard]]
    static StreamableEvent * event_from_stream( const RemoteMessage & stream, const Channel & comChannel );

    /**
     * \brief   Serializes remote event object to streaming object. The specified event must be
     *          remote type.
     *
     * \param   stream              The streaming object to serialize data.
     * \param   eventStreamable     The event object, must be remote event type. Otherwise,
     *                              serialization is ignored.
     * \param   comChannel          The communication channel object to send event.
     * \return  Returns true if successfully recognized remote object and serialized to stream.
     *          Otherwise returns false.
     **/
    static bool stream_from_event( RemoteMessage & stream, const StreamableEvent & eventStreamable, const Channel & comChannel );

    /**
     * \brief   Creates request failure remote event. Called when system fails to process request.
     *
     * \param   stream          The remote messaging streaming object to serialize failure
     *                          information.
     * \param   comChannel      The communication channel object to send event.
     * \return  Returns streamable event object to send to target object.
     **/
    [[nodiscard]]
    static StreamableEvent * request_failed_event( const RemoteMessage & stream, const Channel & comChannel );

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
