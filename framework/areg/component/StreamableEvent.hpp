#ifndef AREG_COMPONENT_STREAMABLEEVENT_HPP
#define AREG_COMPONENT_STREAMABLEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/StreamableEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Streamable communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"

//////////////////////////////////////////////////////////////////////////
// StreamableEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Streamable Event is communication event and provides 
 *          possibility to serialize and deserialize event data in the 
 *          streaming object. All Events used to transfer data are
 *          declared as streamable Event. The Streamable Event is not
 *          created and used directly. Instead, one of derived classes
 *          should be used.
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
    DECLARE_RUNTIME_EVENT(StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates streamable event and defines event type.
     * \param   eventType   The type of event.
     **/
    explicit StreamableEvent( Event::eEventType eventType );

    /**
     * \brief   Creates streamable event and initialize data
     * \param   stream  The streaming object to read data
     **/
    StreamableEvent( const IEInStream & stream );

    /**
     * \brief   Destructor
     **/
    virtual ~StreamableEvent( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// StreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & readStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & writeStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    StreamableEvent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( StreamableEvent );
};

#endif  // AREG_COMPONENT_STREAMABLEEVENT_HPP
