#ifndef AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
#define AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Remote Event Factory class. 
 *              Creates event from stream and converts event to stream.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

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
 * \brief   The remote event factory is creating event objects
 *          for communication from stream or serialize event
 *          object data to the stream to forward to target.
 *          Only remote events will be serialized and instantiated.
 *          The type of remote object depends on data in streaming
 *          object and depending of type, appropriate object
 *          will be constructed.
 **/
class AREG_API RemoteEventFactory
{
//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to create event from streaming object.
     *          The streaming object should be valid and the event data
     *          should be for remote event. Otherwise, no event object
     *          will be created. After processing event, it should is
     *          automatically deleted by dispatcher.
     * \param   stream          The streaming object containing event data.
     * \param   comChannel      The communication channel object to send event.
     * \return  If function succeeds, the return value is valid pointer
     *          to one of remote event objects, which type can be found
     *          by runtime casting. Otherwise, if streaming data contains
     *          wrong or unsupported event types, function returns nullptr.
     **/
    static StreamableEvent * createEventFromStream( const RemoteMessage & stream, const Channel & comChannel );

    /**
     * \brief   Call to serialize remote event object to streaming object.
     *          The specified event should be remote type. Otherwise, event data
     *          will not be serialized.
     * \param   stream          The streaming object to serialize data.
     * \param   eventStreamable The event object, should be remote event type.
     *                          Otherwise, serialization is ignored.
     * \param   comChannel      The communication channel object to send event.
     * \return  Returns true if successfully recognized remote object and could
     *          serialize to streaming object. Otherwise, it returns false.
     **/
    static bool createStreamFromEvent( RemoteMessage & stream, const StreamableEvent & eventStreamable, const Channel & comChannel );

    /**
     * \brief   Call to create request failure remote event. It is called when system failed to processed request.
     * \param   stream          The remote messaging streaming object to serialize failure information.
     * \param   comChannel      The communication channel object to send event.
     * \return  Returns streamable event object to send to target object.
     **/
    static StreamableEvent * createRequestFailedEvent( const RemoteMessage & stream, const Channel & comChannel );

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Hidden
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Default constructor. Hidden.
     *          Object cannot be instantiated directly.
     **/
    RemoteEventFactory( void ) = default;

    /**
     * \brief   Destructor. The object cannot be deleted directly.
     **/
    ~RemoteEventFactory( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( RemoteEventFactory );
};

#endif  // AREG_COMPONENT_REMOTEEVENTFACTORY_HPP
