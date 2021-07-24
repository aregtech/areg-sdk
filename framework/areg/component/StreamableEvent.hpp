#ifndef AREG_COMPONENT_STREAMABLEEVENT_HPP
#define AREG_COMPONENT_STREAMABLEEVENT_HPP
/************************************************************************
 * \file        areg/component/StreamableEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Streamable communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/private/ThreadEventBase.hpp"

//////////////////////////////////////////////////////////////////////////
// StreamableEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Streamable Event is communication event and provides 
 *          possibility to serialize and de-serialize event data in the 
 *          streaming object. All Events used to transfer data are
 *          declared as streamable Event. The Streamable Event is not
 *          created and used directly. Instead, one of derived classes
 *          should be used.
 **/
class AREG_API StreamableEvent : public ThreadEventBase
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
    StreamableEvent( Event::eEventType eventType );

    /**
     * \brief   Creates streamable event and initialize data
     * \param   stream  The streaming object to read data
     **/
    StreamableEvent( const IEInStream & stream );

    /**
     * \brief   Destructor
     **/
    virtual ~StreamableEvent( void );

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
    StreamableEvent( void );
    StreamableEvent( const StreamableEvent & /*src*/ );
    const StreamableEvent & operator = ( const StreamableEvent & /*src*/ );
};

#endif  // AREG_COMPONENT_PRIVATE_STREAMABLEEVENT_HPP
