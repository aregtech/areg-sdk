#ifndef AREG_COMPONENT_CESTREAMABLEEVENT_HPP
#define AREG_COMPONENT_CESTREAMABLEEVENT_HPP
/************************************************************************
 * \file        areg/src/component/CEStreamableEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Streamable communication Event.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/component/private/CEThreadEventBase.hpp"

//////////////////////////////////////////////////////////////////////////
// CEStreamableEvent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Streamable Event is communication event and provides 
 *          possibility to serialize and de-serialize event data in the 
 *          streaming object. All Events used to transfer data are
 *          declared as streamable Event. The Streamable Event is not
 *          created and used directly. Instead, one of derived classes
 *          should be used.
 **/
class AREG_API CEStreamableEvent : public CEThreadEventBase
{
//////////////////////////////////////////////////////////////////////////
// friend classes
//////////////////////////////////////////////////////////////////////////
    friend class CERemoteEventFactory;
//////////////////////////////////////////////////////////////////////////
// Runtime declare
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CEStreamableEvent)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates streamable event and defines event type.
     * \param   eventType   The type of event.
     **/
    CEStreamableEvent( CEEvent::eEventType eventType );

    /**
     * \brief   Creates streamable event and initialize data
     * \param   stream  The streaming object to read data
     **/
    CEStreamableEvent( const IEInStream & stream );

    /**
     * \brief   Destructor
     **/
    virtual ~CEStreamableEvent( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// CEStreamableEvent overrides
/************************************************************************/
    /**
     * \brief   Reads and initialize event data from streaming object.
     * \param   stream  The streaming object to read out event data
     * \return  Returns streaming object to read out data.
     **/
    virtual const IEInStream & ReadFromStream( const IEInStream & stream );

    /**
     * \brief   Writes event data to streaming object
     * \param   stream  The streaming object to write event data.
     * \return  Returns streaming object to write event data.
     **/
    virtual IEOutStream & WriteToStream( IEOutStream & stream ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEStreamableEvent( void );
    CEStreamableEvent( const CEStreamableEvent & /*src*/ );
    const CEStreamableEvent & operator = ( const CEStreamableEvent & /*src*/ );
};

#endif  // AREG_COMPONENT_PRIVATE_CESTREAMABLEEVENT_HPP
